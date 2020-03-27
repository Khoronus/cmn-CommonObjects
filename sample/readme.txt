TrackObserved:
 - It takes the skeleton informations and associate an ID to each of them. The ID is preserved among frames by calculating the minimum distance between centroids.


ExtractActionTag:
 - Extract action information by using the Tag data (instead of the txt)



> TagSource
Program to tag the IN and OUT of a source. It creates a text serialized file.
example: TagSource.exe  "D:\DB\Test\20200120_094850\color\" ".jpg" 0

> NaiveTag_Golf
Program to naively tag a source
example: NaiveTag_Golf.exe "D:\DB\Test\20200120_094850"
It reads the file
<path>/scene_info.txt 
 action_in action_out
<path>/scene_info_actions.txt
 action action_id0 action_in0 action_out0 action_id1 action_in1 action_out1 ...
labels_action.txt
 action_id|action name
The program generates a video file with the expected action recognition. <path>/auto_action_recognition.avi.

> TransformSourceListBinary2EncodeImage
  OpenCVDecodeSource
example: TransformSourceListBinary2EncodeImage.exe 1280 720 0 list_rgb.txt list_depth.txt 
The example keep the source. To delete
example: TransformSourceListBinary2EncodeImage.exe 1280 720 1 list_rgb.txt list_depth.txt 
It transform a set of binary files (saved without encode) in a encoded images. For example saving cv::Mat.data field with std::ofstream
dir color\*.* /s /b > list_rgb.txt     
dir depth\*.* /s /b > list_depth.txt

> TrackObserved
It shows an example of tracking of an observed skeleton.
The data is expected not to be packed (saved frame by frame in <path>/color <path>/skeleton).
example: TrackObserved.exe "D:\DB\Test\20200120_094850"

> TestSerialize
It tests the serialization for the skeleton data. ONLY TEST

> TestReadSceneBinaryData
It test the reading of the binary data which contains the information of the skeleton and rgb.
It is to read the packed data. Expected to find the file
<path>/skeleton.data
<path>/video.avi
example: TestReadSceneBinaryData.exe "D:\DB\Test\20200317_163412" 1
if do_save_output == 1 it saves the packed data frame_by_frame

> TestReadSkeletonBinaryData
example: TestReadBinaryData.exe "D:\DB\Test\20200317_163412\skeleton.data"
Read the binary packed data of a skeleton


> OverlapSequences
It overlaps two sequences with blending effect.
example: OverlapSequences.exe "D:\DB\Test\20200106_134506" 1 "D:\DB\Test\20191227_110526" 1 "videocombo.avi" 1 1
Check the program (run without arguments) for help. 
IMPORTANT: The video sequences MUST have the same size.

> ExtractStartPosition_Golf
It extracts the start position of a golf ball. It uses GrabCut to detect the ball position.
example: ExtractStartPosition_Golf.exe "D:\DB\Test\20200317_163412"
A more accurate algorithm is in the golf project.

> ExtractSkeletonJointsInformation
It extracts the joints information of a skeleton. It used the scene_info.txt to get the IN and OUT of a video.
It uses the currently observed skeleton for the measurement.
It saves graph information as image and observed data as txt.
example: ExtractSkeletonJointsInformation.exe "D:\DB\Test\20200317_163412"

> ExtractPointCloud
It extract and save a point cloud in a naive and XML format. Naive is a set of points as txt file, XML is for the matching viewer (saved as OpenCV XML file).
The input files are expected to be as single frames (<path>/color/000000.jpg for RGB and <path>/depth/000000.png for DEPTH). Intrinsic camera parameters is expected to exist in RealSense format.
example: ExtractPointCloud.exe "D:\DB\Test\20191227_110526" 100 10 "D:\DB\Test\20191227_110526\pointcloud.txt" "D:\DB\Test\20191227_110526\pointcloud.xml"

> ExtractFrames
It extract RGB frames from a video. The data is saved in a destination folder as follow:
<path_dest>/filename_as_folder/color
example: ExtractFrames.exe "d:\DB\Test\20200317_163412\video.avi" "D:\DB\Test\TestFile"

> ExtractActionTag
It extracts the action information from a previously tag scene (see TagSource)
example: ExtractActionTag.exe "D:\DB\Test\20200120_094850" "scene_tag.txt" "0_0"

> DrawSkeleton
It draws the skeletons in the scene. It saves <path>/skeletons.avi as result.
example: DrawSkeleton.exe "D:\DB\Test\20200120_094850"

> CompareKeyFrame
It analyzes the content of a folder and it creates the sequence information.
It uses the unpacked data (<path>/skeleton) and it saves the sequence of an observed skeleton.
It creates <path>/sequence2D.txt
           <path>/sequence3D.txt
example: CompareKeyFrame.exe "D:\DB\Test\20200120_094850"

> CompareHistogramTwoHumans
It compares the result of the "ExtractSkeletonJointsInformation" where histograms information are created. The label represents which joint pair is compared.
It compares the similarity between two histograms.
example: CompareHistogramTwoHumans.exe "D:\DB\Test\20200317_163412" "D:\DB\Test\20200317_163412" label1

> ExtractAction
It extracts the action information from a scene_info.txt data.
It create the observer.txt file with the observed skeleton information.
It saves the extracted action information in extract_action folder.
example: ExtractAction.exe "D:\DB\Test\20200317_163412" "0_1"

> AutoTag
It automatically tags a scene.
example: AutoTag.exe "D:\DB\Test\20200317_163412"
The program expects to connect to the sequence_recognition program in order to recognize an action sequence.

> ExtractAction_Golf
It requires the information of scene_info_actions.txt
It create the observer.txt file with the observed skeleton information.
It saves the extracted action information in extract_action folder.
example: ExtractAction_golf.exe "D:\DB\Test\20200120_094850" "0_2"

> AutoTag_Golf
It automatically tags a scene.
example: AutoTag_golf.exe "D:\DB\Test\20200317_163412"
The program expects to connect to the sequence_recognition program in order to recognize an action sequence.
It crash if it cannot connect (check the autotag).

