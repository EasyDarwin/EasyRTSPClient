## EasyRTSPClient ##

### EasyRTSPClient是什么？ ###

EasyRTSPClient是EasyDarwin开源流媒体团队提供的一套非常稳定、易用、支持重连的RTSPClient工具，接口调用非常简单，再也不用像调用live555那样处理整个RTSP OPTIONS/DESCRIBE/SETUP/PLAY的复杂流程，担心内存释放的问题了！我们提供了多个调用示例：

- **EasyRTSPClient**：以RTSPClient的形式，从RTSP URL将音视频获取到本地；
	
	Windows编译方法，

    	Visual Studio 2010 编译：./EasyRTSPClient-master/win/EasyRTSPClient.sln

	Linux编译方法，
		
		chmod +x ./Buildit
		./Buildit


- **EasyDarwin**：您也可以参考EasyDarwin中EasyHLSSession(HLS直播模块)、EasyRelaySession(RTSP转发模块)对EasyRTSPClient库的调用方法，详细请看：[https://github.com/EasyDarwin/EasyDarwin](https://github.com/EasyDarwin/EasyDarwin "EasyDarwin")；

- **我们同时提供Windows、Linux、ARM版本的libEasyRTSPClient库**：arm版本请将交叉编译工具链发送至support@easydarwin.org，我们会帮您具体编译

### 调用流程 ###
![](http://www.easydarwin.org/skin/easydarwin/images/easyrtspclient.png)


### RTSPSourceCallBack数据回调说明 ###
EasyRTSPClient可以回调出多种类型的数据：

	#define EASY_SDK_VIDEO_FRAME_FLAG			/* 视频帧数据 */
	#define EASY_SDK_AUDIO_FRAME_FLAG			/* 音频帧数据 */
	#define EASY_SDK_EVENT_FRAME_FLAG			/* 事件帧数据 */
	#define EASY_SDK_RTP_FRAME_FLAG				/* RTP帧数据 */
	#define EASY_SDK_SDP_FRAME_FLAG				/* SDP帧数据 */
	#define EASY_SDK_MEDIA_INFO_FLAG			/* 媒体类型数据 */

EASY\_SDK\_VIDEO\_FRAME\_FLAG数据可支持多种视频格式：
		
	#define EASY_SDK_VIDEO_CODEC_H264			/* H264  */
	#define	EASY_SDK_VIDEO_CODEC_MJPEG			/* MJPEG */
	#define	EASY_SDK_VIDEO_CODEC_MPEG4			/* MPEG4 */


> ***当回调出RTSP_FRAME_INFO->codec为EASY\_SDK\_VIDEO\_CODEC\_H264数据，RTSP_FRAME_INFO->type为EASY\_SDK\_VIDEO\_FRAME\_I关键帧时，我们输出的数据结构为SPS+PPS+IDR的组合***：
		
		|---------sps---------|-------pps-------|-----------------idr-----------------|
		|                     |                 |                                     |
		0-----------------reserved1---------reserved2-------------------------------length


EASY\_SDK\_AUDIO\_FRAME\_FLAG数据可支持多种音频格式：
	
	#define EASY_SDK_AUDIO_CODEC_AAC			/* AAC */
	#define EASY_SDK_AUDIO_CODEC_G711A			/* G711 alaw*/
	#define EASY_SDK_AUDIO_CODEC_G711U			/* G711 ulaw*/
	#define EASY_SDK_AUDIO_CODEC_G726			/* G726 */




----------

### 获取更多信息 ###

邮件：[support@easydarwin.org](mailto:support@easydarwin.org) 

WEB：[www.EasyDarwin.org](http://www.easydarwin.org)

Copyright (c) EasyDarwin.org 2012-2015

![EasyDarwin](http://www.easydarwin.org/skin/easydarwin/images/wx_qrcode.jpg)