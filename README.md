# EasyRTSPClient v3.0 #

EasyRTSPClient是[TSINGSEE青犀开放平台](http://www.tsingsee.com "TSINGSEE青犀开放平台")开发和提供的一套非常稳定、易用、支持重连的RTSPClient组件，帮助用户非常简单地处理整个RTSP OPTIONS/DESCRIBE/SETUP/PLAY的复杂流程，担心内存释放的问题了，EasyRTSPClient全平台支持（包括Windows/Linux 32&64，ARM各种平台，Android，iOS），是技术研发快速迭代的工具，也是安防运维人员进行现场问题排查的得力帮手！

## 调用示例 ##

- **EasyRTSPClient**：以RTSPClient的形式，从RTSP URL将音视频获取到本地；
	
	Windows编译方法，

    	Visual Studio 2010 编译：./EasyRTSPClient-master/win/EasyRTSPClient.sln

	Linux编译方法，
		
		chmod +x ./Buildit
		./Buildit

	Usage，
		
		--------------------------------------------------------------
		EasyRTSPClient.exe -d <rtsp-url>[ -m <transport-mode> -s <save-file>]
		Help Mode:   EasyRTSPClient.exe -h
		rtsp-url : source rtsp address
		transport-mode : tcp or udp, default is tcp
		save-file : yes or no, default is yes
		For example: EasyRTSPClient.exe -d rtsp://admin:admin@192.168.2.100/11 -m tcp -s yes
		--------------------------------------------------------------


- **EasyPlayer调用**：EasyPlayer是由EasyDarwin开源团队开发和维护的一个RTSP播放器项目，目前支持Windows(支持多窗口、包含ActiveX，npAPI Web插件)、Android平台，iOS平台，EasyRTSPClient作为其RTSP/RTP部分组件，为EasyPlayer提供稳定可靠、高兼容性的数据连接；

- **EasyClient客户端**：EasyClient是EasyDarwin开源流媒体云平台的客户端实现，包含：Windows、Android、iOS三个部分，EasyRTSPClient为EasyClient提供RTSP/RTP/RTCP协议支撑；

	<table>
	<tr><td><b>支持平台</b></td><td><b>芯片</b></td><td><b>目录位置</b></td></tr>
	<tr><td>Windows</td><td>x86</td><td>./Lib/</td></tr>
	<tr><td>Windows</td><td>x64</td><td>./Lib/x64/</td></tr>
	<tr><td>Linux</td><td>x86</td><td>./Lib/</td></tr>
	<tr><td>Linux</td><td>x64</td><td>./Lib/x64/</td></tr>
	<tr><td>海思</td><td>arm-hisiv100-linux</td><td>./Lib/hisiv100/</td></tr>
	<tr><td>海思</td><td>arm-hisiv200-linux</td><td>./Lib/hisiv200/</td></tr>
	<tr><td>海思</td><td>arm-hisiv300-linux</td><td>./Lib/hisiv300/</td></tr>
	<tr><td>海思</td><td>arm-hisiv400-linux</td><td>./Lib/hisiv400/</td></tr>
	<tr><td>海思</td><td>arm-hisiv500-linux</td><td>./Lib/hisiv500/</td></tr>
	<tr><td>海思</td><td>arm-hisiv600-linux</td><td>./Lib/hisiv600/</td></tr>
	<tr><td>Android</td><td>arm/arm64/x86/x86_64/mips/mips_64</td><td>arm/arm64/x86/x86_64/mips/mips_64</td></tr>
	<tr><td>iOS</td><td>arm/arm64</td><td>https://github.com/EasyDSS/EasyPlayer-RTSP-iOS</td></tr>

	<tr><td colspan="3"><center>邮件获取更多平台版本</center></td></tr>
	</table>

## 调用流程 ##
![](http://www.easydarwin.org/skin/easydarwin/images/easyrtspclient20160909.gif)

### RTSPSourceCallBack数据回调说明 ###
EasyRTSPClient可以回调出多种类型的数据：

	#define EASY_SDK_VIDEO_FRAME_FLAG			/* 视频帧数据 */
	#define EASY_SDK_AUDIO_FRAME_FLAG			/* 音频帧数据 */
	#define EASY_SDK_EVENT_FRAME_FLAG			/* 事件帧数据 */
	#define EASY_SDK_RTP_FRAME_FLAG				/* RTP帧数据 */
	#define EASY_SDK_SDP_FRAME_FLAG				/* SDP帧数据 */
	#define EASY_SDK_MEDIA_INFO_FLAG			/* 媒体类型数据 */

EASY\_SDK\_VIDEO\_FRAME\_FLAG数据可支持多种视频格式：
		
	#define EASY_SDK_VIDEO_CODEC_H265			/* H265  */
	#define EASY_SDK_VIDEO_CODEC_H264			/* H264  */
	#define	EASY_SDK_VIDEO_CODEC_MJPEG			/* MJPEG */
	#define	EASY_SDK_VIDEO_CODEC_MPEG4			/* MPEG4 */

视频帧标识支持

	#define EASY_SDK_VIDEO_FRAME_I				/* I帧 */
	#define EASY_SDK_VIDEO_FRAME_P				/* P帧 */
	#define EASY_SDK_VIDEO_FRAME_B				/* B帧 */
	#define EASY_SDK_VIDEO_FRAME_J				/* JPEG */


> ***当回调出RTSP_FRAME_INFO->codec为EASY\_SDK\_VIDEO\_CODEC\_H264数据，RTSP_FRAME_INFO->type为EASY\_SDK\_VIDEO\_FRAME\_I关键帧时，我们输出的数据结构为SPS+PPS+I的组合***：
		
		|---------sps---------|-------pps-------|---------------I Frame---------------|
		|                     |                 |                                     |
		0-----------------reserved1---------reserved2-------------------------------length


EASY\_SDK\_AUDIO\_FRAME\_FLAG数据可支持多种音频格式：
	
	#define EASY_SDK_AUDIO_CODEC_AAC			/* AAC */
	#define EASY_SDK_AUDIO_CODEC_G711A			/* G711 alaw*/
	#define EASY_SDK_AUDIO_CODEC_G711U			/* G711 ulaw*/
	#define EASY_SDK_AUDIO_CODEC_G726			/* G726 */

## 获取更多信息 ##

TSINGSEE青犀开放平台：[http://open.tsingsee.com](http://open.tsingsee.com "TSINGSEE青犀")

Copyright &copy; TSINGSEE.com 2012~2019