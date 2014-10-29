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
string GetFileName(string knownPath)
{
	// HRESULT hr;
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	string path = knownPath + "roomID100_sensor0_" + to_string(timeinfo->tm_year % 100 + 2000) + "_" + to_string(timeinfo->tm_mon) + "_" + to_string(timeinfo->tm_mday) + "_"
		+ to_string(timeinfo->tm_hour) + "_" + to_string(timeinfo->tm_min) + "_" + to_string(timeinfo->tm_sec) + ".avi";
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

int main(int argc, char** argv)
{
	string knownPath = ".\\KinectData\\";

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
				 FrameSourceTypes::FrameSourceTypes_Infrared,
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
	int FramesPerFile = 10;
	int FileSizeInMS = 900;
	int m_TimerCount = 0;

	// Initialize RGBStream Setting
	int width = 1920;   
    int height = 1080;  
    int bufferSize = width * height * sizeof(RGBQUAD); 
	Mat *bufferMat = NULL;
	//VideoWriter video = NULL;

    Mat colorMat(height / 2, width / 2, CV_8UC4); 
    namedWindow( "Color" );
	
	//RGBQUAD *pBuffer = new RGBQUAD[width * height]; /// new RGBQUAD[width * height];
	
	int count = 0;
	//int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
	VideoWriter *video = new VideoWriter(".\\123.avi",
		9,
		//VideoWriter::fourcc('D', 'I', 'V', 'X'),
		10,
		Size(width, height),
		true);

	//m_TimerCount %= FileSizeInMS;
    while(count < 200)
	{
		//GetFileName(knownPath);
		IColorFrame* pColorFrame = NULL;
		IMultiSourceFrame* pMultiSourceFrame = NULL;
		IColorFrameReference* pColorFrameReference = NULL;
		RGBQUAD *pBuffer = NULL;
		hResult = m_pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);
		if (FAILED(hResult))
		{
			continue;
		}
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
		
		if( SUCCEEDED(hResult))
		{
			//if ((m_TimerCount++) % FileSizeInMS == 0)
			//{
				//string filename = GetFileName(knownPath);
				//cout << filename << endl;
			count++;
			cout << count << endl;
				//if (m_TimerCount == 0)
				//	video->release();
			//}
			//pBuffer = new RGBQUAD[width * height];
			// new RGBQUAD[width * height];
			pBuffer = new RGBQUAD[width * height];
			BYTE* m_pBuffer = new BYTE[width * height * 4];
			hResult = pColorFrame->CopyConvertedFrameDataToArray(bufferSize, reinterpret_cast<BYTE*>(pBuffer), ColorImageFormat_Bgra);
			
			memcpy_s(m_pBuffer, width * height * 4, reinterpret_cast<BYTE*>(pBuffer), bufferSize);
			//if (video != NULL)
			//{		
			if(SUCCEEDED( hResult ))
			{	
				bufferMat = new Mat(height, width, CV_8UC4, m_pBuffer);
				video->write(*bufferMat);
				//resize(bufferMat, colorMat, cv::Size(), 0.5, 0.5);	
				//}
				imshow("Color", *bufferMat);
				if (waitKey(30) == VK_ESCAPE)
				{
					break;
				}
				if (bufferMat)
				{
					// delete[] m_pBuffer;
					delete[] pBuffer;
					delete bufferMat;
				}
				//resize( bufferMat, colorMat, cv::Size(), 0.5, 0.5);				
			}
		}
		SafeRelease( pColorFrame );	
	}
	// SafeRelease( pColorSource );
	// SafeRelease(m_pMultiSourceFrameReader);
	// SafeRelease(pDescription);
	if(pSensor)
	{
		pSensor->Close();
	}
	SafeRelease(pSensor);
	//destroyAllWindows();
	video->release();
	return 0;
}

