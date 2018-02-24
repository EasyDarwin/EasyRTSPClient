# EasyRTSPClient SDK #

EasyRTSPClient是[紫鲸云](http://www.pvale.com "紫鲸云")团队开发、提供的一套非常稳定、易用、支持重连的RTSPClient工具，SDK形式提供，接口调用非常简单，再也不用像调用live555和ffmpeg那样处理整个RTSP OPTIONS/DESCRIBE/SETUP/PLAY的复杂流程，担心内存释放的问题了，EasyRTSPClient全平台支持（包括Windows/Linux 32&64，ARM各种平台，Android，iOS），接口简单且成熟稳定！

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
	<tr><td>Android</td><td>armeabi</td><td>libEasyRTSPClient.so：https://github.com/EasyDarwin/EasyPlayer_Android</td></tr>
	<tr><td>iOS</td><td>arm</td><td>libEasyRTSPClient.a：https://github.com/EasyDarwin/EasyPlayer_iOS</td></tr>
	<tr><td>arago</td><td>arm-arago-linux</td><td>./Lib/arago/</td></tr>
	<tr><td colspan="3"><center>邮件获取更多平台版本</center></td></tr>
	</table>

## 调用流程 ##
![](http://www.easydarwin.org/skin/easydarwin/images/easyrtspclient20160909.gif)


## 设计方法 ##
EasyRTSPClient参考live555 testProg中的testRTSPClient示例程序，将一个live555 testRTSPClient封装在一个类中，例如，我们称为Class EasyRTSPClient，在EasyRTSP_Init接口调用时，我们新建EasyRTSPClient对象、在EasyRTSP_OpenStream接口调用时，我们建立线程，装载live555的TaskScheduler->SingleStep(0)，然后再进行RTSP的具体流程，这个就可以直接用testRTSPClient的使用流程了、关闭RTSPClient，我们调用EasyRTSP_CloseStream接口，内部实现参考testRTSPClient中的shutdownStream方法，最后delete EasyRTSPClient类，这样整个过程就完整了！

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


## 技术支持 ##

- 邮件：[support@easydarwin.org](mailto:support@easydarwin.org) 

- Tel：13718530929

- QQ交流群：[544917793](http://jq.qq.com/?_wv=1027&k=2IDkJId "EasyPlayer")

> **我们同时提供Windows、Linux、ARM、Android、iOS版本的EasyRTSPClient库**：EasyRTSPClient SDK商业使用需要经过授权才能永久使用，商业授权方案可以通过以上渠道进行更深入的技术与合作咨询；


## 获取更多信息 ##

**EasyDarwin**开源流媒体服务器：[www.EasyDarwin.org](http://www.easydarwin.org)

**EasyDSS**商用流媒体解决方案：[www.EasyDSS.com](http://www.easydss.com)

**EasyNVR**无插件直播方案：[www.EasyNVR.com](http://www.easynvr.com)

Copyright &copy; EasyDarwin Team 2012-2018

![EasyDarwin](http://www.easydarwin.org/skin/easydarwin/images/wx_qrcode.jpg)
