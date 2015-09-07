/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#define _CRTDBG_MAP_ALLOC
#include <stdio.h>
#include "EasyRTSPClientAPI.h"
#include <windows.h>

#define RTSPURL "rtsp://admin:admin@wellsen.f3322.org/"

Easy_RTSP_Handle fRTSPHandle = 0;

/* RTSPClient数据回调 */
int Easy_APICALL __RTSPClientCallBack( int _chid, int *_chPtr, int _frameType, char *_pBuf, RTSP_FRAME_INFO* _frameInfo)
{
	if (_frameType == EASY_SDK_VIDEO_FRAME_FLAG)//回调视频数据
	{
		if (_frameInfo->codec == EASY_SDK_VIDEO_CODEC_H264)
			printf("Get %s H264 Len:%d \ttimestamp:%u.%u\n",_frameInfo->type==EASY_SDK_VIDEO_FRAME_I?"I":"P", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
		else if (_frameInfo->codec == EASY_SDK_VIDEO_CODEC_MJPEG)
			printf("Get MJPEG Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
		else if (_frameInfo->codec == EASY_SDK_VIDEO_CODEC_MPEG4)
			printf("Get MPEG4 Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
	}
	else if (_frameType == EASY_SDK_AUDIO_FRAME_FLAG)//回调音频数据
	{
		if (_frameInfo->codec == EASY_SDK_AUDIO_CODEC_AAC)
			printf("Get AAC Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
		else if (_frameInfo->codec == EASY_SDK_AUDIO_CODEC_G711A)
			printf("Get PCMA Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
		else if (_frameInfo->codec == EASY_SDK_AUDIO_CODEC_G711U)
			printf("Get PCMU Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
		else if (_frameInfo->codec == EASY_SDK_AUDIO_CODEC_G726)
			printf("Get G726 Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
	}
	else if (_frameType == EASY_SDK_EVENT_FRAME_FLAG)//回调连接状态事件
	{
		if (NULL == _pBuf && NULL == _frameInfo)
		{
			printf("Connecting:%s ...\n", RTSPURL);
		}
		else if (NULL!=_frameInfo && _frameInfo->type==0xF1)
		{
			printf("Lose Packet:%s ...\n", RTSPURL);
		}
	}
	else if (_frameType == EASY_SDK_MEDIA_INFO_FLAG)//回调出媒体信息
	{
		if(_pBuf != NULL)
		{
			EASY_MEDIA_INFO_T mediainfo;
			memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
			memcpy(&mediainfo, _pBuf, sizeof(EASY_MEDIA_INFO_T));
			printf("RTSP DESCRIBE Get Media Info: video:%u fps:%u audio:%u channel:%u sampleRate:%u \n", 
				mediainfo.u32VideoCodec, mediainfo.u32VideoFps, mediainfo.u32AudioCodec, mediainfo.u32AudioChannel, mediainfo.u32AudioSamplerate);
		}
	}
	return 0;
}

int main()
{
	//创建RTSPSource
	EasyRTSP_Init(&fRTSPHandle);

	// 可以根据fRTSPHanlde判断，也可以根据EasyRTSP_Init是否返回0判断
	if (NULL == fRTSPHandle) return 0;
	
	// 设置数据回调处理
	EasyRTSP_SetCallback(fRTSPHandle, __RTSPClientCallBack);

	// 设置接收音频、视频数据
	unsigned int mediaType = EASY_SDK_VIDEO_FRAME_FLAG | EASY_SDK_AUDIO_FRAME_FLAG;

	// 打开RTSP流
	EasyRTSP_OpenStream(fRTSPHandle, 0, RTSPURL, RTP_OVER_TCP, mediaType, 0, 0, NULL, 1000, 0);

	getchar();
   
	// 关闭RTSPClient
	EasyRTSP_CloseStream(fRTSPHandle);

	// 释放RTSPHandle
	EasyRTSP_Deinit(&fRTSPHandle);
	fRTSPHandle = NULL;

    return 0;
}