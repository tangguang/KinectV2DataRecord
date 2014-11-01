#include <Windows.h>
#include <Kinect.h>
#include <iostream>
#include <time.h>

#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  

using namespace std;
using namespace cv;


template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
    if (pInterfaceToRelease != NULL)
    {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = NULL;
    }
}

// HRESULT GetFileName(string FileName, string knownPath, int senserID)
string GetFileName(string knownPath, int sensorID)
{
	// HRESULT hr;
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	string path;
	switch (sensorID)
	{
	case 0:
		path = knownPath + "100_0_" + to_string(timeinfo->tm_year % 100 + 2000) + "_" + to_string(timeinfo->tm_mon + 1) + "_" + to_string(timeinfo->tm_mday) + "_"
			+ to_string(timeinfo->tm_hour) + "_" + to_string(timeinfo->tm_min) + "_" + to_string(timeinfo->tm_sec) + ".avi";
		break;
	case 1:
		path = knownPath + "100_1_" + to_string(timeinfo->tm_year % 100 + 2000) + "_" + to_string(timeinfo->tm_mon + 1) + "_" + to_string(timeinfo->tm_mday) + "_"
			+ to_string(timeinfo->tm_hour) + "_" + to_string(timeinfo->tm_min) + "_" + to_string(timeinfo->tm_sec) + ".avi";
		break;
	case 2:
		path = knownPath + "100_2_" + to_string(timeinfo->tm_year % 100 + 2000) + "_" + to_string(timeinfo->tm_mon + 1) + "_" + to_string(timeinfo->tm_mday) + "_"
			+ to_string(timeinfo->tm_hour) + "_" + to_string(timeinfo->tm_min) + "_" + to_string(timeinfo->tm_sec) + ".csv";
		break;
	}
	
	//cout << path << endl;
	return path;

	//// get the time
	//wchar_t timestring[max_path];
	//wchar_t datestring[max_path];
	//gettimeformatex(null, time_force24hourformat, null, l"hh'-'mm'-'ss", timestring, _countof(timestring));
	//getdateformatex(null, 0, null, l"yyyy'-'mmm'-'dd", datestring, _countof(datestring), null);

	//// file name will be deviceconnectid-kinectaudio-hh-mm-ss.wav
	//switch (senserid)
	//{
	//case 1:
	//	hr = stringcchprintfw(filename, filenamesize, l"%s\\kinectaudio\\audio-%s-%s-%s.wav", knownpath, devidout, datestring, timestring);
	//	break;
	//case 2:
	//	hr = stringcchprintfw(filename, filenamesize, l"%s\\kinectrgb\\rgb-%s-%s-%s.avi", knownpath, devidout, datestring, timestring);
	//	break;
	//case 3:
	//	hr = stringcchprintfw(filename, filenamesize, l"%s\\kinectdepth\\depth-%s-%s-%s.avi", knownpath, devidout, datestring, timestring);
	//	break;
	///*case speechrecog:
	//	hr = stringcchprintfw(filename, filenamesize, l"%s\\kinectspeechrecog\\speechrecog-%s-%s-%s.txt", knownpath, devidout, datestring, timestring);
	//	break;*/
	//default:
	//	break;
	//}
	//return hr;
}

void ProcessDepth(const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth, VideoWriter *depth);

int main(int argc, char** argv)
{
	string knownPath = ".\\";

	// Initialize Kinect Sensor
	IKinectSensor* pSensor = NULL;
	IMultiSourceFrameReader* m_pMultiSourceFrameReader = NULL;
    HRESULT hResult = S_OK;
	hResult = GetDefaultKinectSensor(&pSensor);
	if( FAILED( hResult ) )
	{
		cout << "Error : GetDefaultKinectSensor" << endl;
        return -1;
    }

	// Open MultipleFrameSource
	hResult = pSensor->Open();
    if( FAILED( hResult ) )
	{
        cout << "Error : IKinectSensor::Open()" << endl;
        return -1;
    }
	hResult = pSensor->OpenMultiSourceFrameReader(
		         FrameSourceTypes::FrameSourceTypes_Depth | 
				 FrameSourceTypes::FrameSourceTypes_Color | 
				 FrameSourceTypes::FrameSourceTypes_Infrared |
				 FrameSourceTypes::FrameSourceTypes_Body,
				 &m_pMultiSourceFrameReader);
	if (FAILED(hResult))
	{
		cout << "Error : IKinectSensor::get_MultiSourceFrameReader()" << endl;
		return -1;
	}

	// Description
	//IFrameDescription* pDescription;
	//hResult = pColorSource->get_FrameDescription( &pDescription );
	//if( FAILED( hResult ) ){
	//	std::cerr << "Error : IColorFrameSource::get_FrameDescription()" << std::endl;
	//	return -1;
	//}

	// File Size Settiing
	int FramesPerFile = 900;
	int m_TimerCount = 0;

	// Initialize RGBStream Setting
	int width = 1920;   
    int height = 1080;  
    int RGBSize = width * height * 4 * sizeof(unsigned char); 
	Mat RGBMat(height, width, CV_8UC4);
	VideoWriter *video = NULL;

	// Initialize DepthStream Setting
	int depthwidth = 512;
	int depthheigh = 424;
	VideoWriter *depth = NULL;
	
	//RGBQUAD *pBuffer = new RGBQUAD[width * height]; /// new RGBQUAD[width * height];
	
	//int count = 0;
	//int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
	

	//m_TimerCount %= FileSizeInMS;
    while(true)
	{
		// Get MultiSource Frame
		IMultiSourceFrame* pMultiSourceFrame = NULL;
		
		// Get Color Frame
		IColorFrame* pColorFrame = NULL;
		IColorFrameReference* pColorFrameReference = NULL;
		 
		// Get Depth Frame
		IDepthFrame* pDepthFrame = NULL;
		IDepthFrameReference* pDepthFrameReference = NULL;
		USHORT nDepthMinReliableDistance = 0;
		USHORT nDepthMaxDistance = 0;
		UINT nBufferSize = 0;
		UINT16 *pDepthBuffer = NULL;
		
		// Get Body Frame
		IBodyFrame* pBodyFrame = NULL;
		IBodyFrameReference* pBodyFrameReference = NULL;

		// Multi Source
		hResult = m_pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);
		if (FAILED(hResult))
		{
			continue;
		}
		// RGB
		hResult = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
		if (FAILED(hResult))
		{
			continue;
		}
		hResult = pColorFrameReference->AcquireFrame(&pColorFrame);
		if (FAILED(hResult))
		{
			continue;
		}
		// Depth
		hResult = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
		if (FAILED(hResult))
		{
			continue;
		}
		hResult = pDepthFrameReference->AcquireFrame(&pDepthFrame);
		if (FAILED(hResult))
		{
			continue;
		}
		// Body
		hResult = pMultiSourceFrame->get_BodyFrameReference(&pBodyFrameReference);

		if( SUCCEEDED(hResult))
		{		
			cout << m_TimerCount << endl;
			if (m_TimerCount == 0)
			{
				string filename1 = GetFileName(knownPath, 0);
				cout << filename1 << endl;
				video = new VideoWriter(filename1,
					9,
					10,
					Size(width, height),
					true);

				string filename2 = GetFileName(knownPath, 1);
				cout << filename2 << endl;
				depth = new VideoWriter(filename2,
					10,
					10,
					Size(depthwidth, depthheigh),
					true);
			}
			m_TimerCount++;
			if (m_TimerCount == FramesPerFile - 1)
			{
				cout << "release" << endl; 
				m_TimerCount = 0;
				video->release();
				depth->release();
			}
			//pBuffer = new RGBQUAD[width * height];
			// new RGBQUAD[width * height];

			//pBuffer = new RGBQUAD[width * height];
			//BYTE* m_pBuffer = new BYTE[width * height * 4];
			hResult = pColorFrame->CopyConvertedFrameDataToArray(RGBSize, reinterpret_cast<BYTE*>(RGBMat.data), ColorImageFormat_Bgra);
			if (SUCCEEDED(hResult))
			{
				video->write(RGBMat);
				/*imshow("Depth", RGBMat);
				if (waitKey(30) == VK_ESCAPE)
				{
					break;
				}*/
			}
			//UINT16 *buffer = new UINT16(depthheigh * dep);
			hResult = pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
			if (SUCCEEDED(hResult))
			{
				if (SUCCEEDED(hResult))
				{
					hResult = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pDepthBuffer);
				}
				if (SUCCEEDED(hResult))
				{
					ProcessDepth(pDepthBuffer, depthwidth, depthheigh, nDepthMinReliableDistance, USHRT_MAX, depth);
				}
			}
			//if (video != NULL)
			//{		
			//bufferMat = new Mat(height, width, CV_8UC4, m_pBuffer);
		    //video->write(RGBMat);
				//resize(bufferMat, colorMat, cv::Size(), 0.5, 0.5);	
				//}
		    
				/*if (bufferMat)
				{
					// delete[] m_pBuffer;
					delete[] pBuffer;
					delete bufferMat;
				}*/
				//resize( bufferMat, colorMat, cv::Size(), 0.5, 0.5);	
			//cout << "wow" << endl;
			//if (m_TimerCount % FramesPerFile == 0)
			//{
				//video->release();
				//depth->release();
			//}
		}
		SafeRelease(pMultiSourceFrame);
		SafeRelease(pColorFrameReference);
		SafeRelease(pDepthFrameReference);
		SafeRelease(pDepthFrame);
		SafeRelease(pColorFrame);	
		//delete pBuffer;
	}
	// SafeRelease( pColorSource );
	// SafeRelease(m_pMultiSourceFrameReader);
	// SafeRelease(pDescription);
	if(pSensor)
	{
		pSensor->Close();
	}
	SafeRelease(pSensor);
	return 0;
}

void ProcessDepth(const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth, VideoWriter *depthvideo)
{
	// End pixel is start + width*height - 1
	Mat depthMat(nHeight, nWidth, CV_8UC1);
	int pBufferLength = nWidth * nHeight;
	int i = 0;
	while (i < pBufferLength)
	{
		USHORT depth = *pBuffer;
		BYTE intensity = static_cast<BYTE>((depth >= nMinDepth) && (depth <= nMaxDepth) ? (depth % 256) : 0);
		++pBuffer;
		depthMat.at<BYTE>(i / nWidth, i % nWidth) = intensity;
		i++;
	}
	depthvideo->write(depthMat);  
}