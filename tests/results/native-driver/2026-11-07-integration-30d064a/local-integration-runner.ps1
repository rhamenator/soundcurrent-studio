$ErrorActionPreference='Stop'
$root=$PSScriptRoot
$build='C:\Users\wyurd\Downloads\SoundCurrent-native-driver-test-4558682\studio-mingw'
$env:PATH='C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.5.3\mingw_64\bin;'+$env:PATH
$env:QT_QPA_PLATFORM='offscreen'
$env:QT_QPA_FONTDIR=Join-Path $env:SystemRoot 'Fonts'
$env:QT_PLUGIN_PATH='C:\Qt\6.5.3\mingw_64\plugins'
$env:QT_FORCE_STDERR_LOGGING='1'
(Get-Process -Id $PID).PriorityClass='BelowNormal'
$source=Get-Content (Join-Path $env:TEMP 'soundcurrent-audio-diagnostics\AudioCheck.cs') -Raw
$source+=@'
[ComImport,Guid("870af99c-171d-4f9e-af0d-e63df40c2bc9")]class PolicyClient{}
[ComImport,Guid("f8679f50-850a-41cf-9c72-430f290290c8"),InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]interface IPolicy{void GetMixFormat();void GetDeviceFormat();void ResetDeviceFormat();void SetDeviceFormat();void GetProcessingPeriod();void GetShareMode();void SetShareMode();void GetPropertyValue();void SetPropertyValue();void SetDefaultEndpoint([MarshalAs(UnmanagedType.LPWStr)]string id,int role);void SetEndpointVisibility();}
public class SavedLevel{public string Id;public float Level;public bool Muted;}
public class AudioSafety{
 public string[] Roles;public SavedLevel[] Levels;
 public static AudioSafety Capture(string[] ids){var e=(IEnum)new Enumerator();var roles=new System.Collections.Generic.List<string>();for(int f=0;f<2;f++)for(int r=0;r<3;r++){IDevice d;e.GetDefaultAudioEndpoint(f,r,out d);string id;d.GetId(out id);roles.Add(f+"|"+r+"|"+id);if(f==0&&r==0){var l=new System.Collections.Generic.List<string>(ids);l.Add(id);ids=l.ToArray();}}var levels=new System.Collections.Generic.List<SavedLevel>();foreach(var id in ids){IDevice d;e.GetDevice(id,out d);Guid g=typeof(IVolume).GUID;object o;d.Activate(ref g,23,IntPtr.Zero,out o);var v=(IVolume)o;var l=new SavedLevel{Id=id};v.GetMasterVolumeLevelScalar(out l.Level);v.GetMute(out l.Muted);levels.Add(l);}return new AudioSafety{Roles=roles.ToArray(),Levels=levels.ToArray()};}
 public void Restore(){var e=(IEnum)new Enumerator();foreach(var l in Levels){IDevice d;e.GetDevice(l.Id,out d);Guid g=typeof(IVolume).GUID;object o;d.Activate(ref g,23,IntPtr.Zero,out o);var v=(IVolume)o;v.SetMasterVolumeLevelScalar(l.Level,IntPtr.Zero);v.SetMute(l.Muted,IntPtr.Zero);}var p=(IPolicy)new PolicyClient();foreach(var s in Roles){var a=s.Split('|');p.SetDefaultEndpoint(a[2],int.Parse(a[1]));}}
 public static string State(){var old=Console.Out;var writer=new System.IO.StringWriter();try{Console.SetOut(writer);AudioCheck.Run();return writer.ToString();}finally{Console.SetOut(old);}}
}
'@
# The policy interface has two processing-period methods before GetShareMode.
$source=$source.Replace('void GetProcessingPeriod();void GetShareMode();','void GetProcessingPeriod();void SetProcessingPeriod();void GetShareMode();')
Add-Type -TypeDefinition $source
$ids=@(Get-PnpDevice -Class AudioEndpoint|Where-Object {$_.Status -eq 'OK' -and $_.FriendlyName -match 'SoundCurrent' -and $_.InstanceId -match '\{0\.0\.0\.'}|ForEach-Object {$_.InstanceId -replace '^SWD\\MMDEVAPI\\',''})
if($ids.Count -ne 2){throw 'Expected exactly two native render endpoints'}
if(Get-Process soundcurrent-studio,soundcurrent-eq,fxsound -ErrorAction SilentlyContinue){throw 'Audio processor already running'}
$ids+=@(Get-PnpDevice -Class AudioEndpoint|Where-Object {$_.Status -eq 'OK' -and $_.FriendlyName -match 'SoundCurrent' -and $_.InstanceId -match '\{0\.0\.1\.'}|ForEach-Object {$_.InstanceId -replace '^SWD\\MMDEVAPI\\',''})
$saved=[AudioSafety]::Capture([string[]]$ids)
$saved|ConvertTo-Json -Depth 5|Set-Content (Join-Path $root 'logs\integration-safety-before.json')
function RunTest([string]$exe,[string]$arguments,[string]$name,[int]$timeout=55000){
 $info=New-Object Diagnostics.ProcessStartInfo
 $info.FileName=Join-Path $build $exe;$info.Arguments=$arguments
 $info.UseShellExecute=$false;$info.RedirectStandardOutput=$true;$info.RedirectStandardError=$true;$info.CreateNoWindow=$true
 $p=New-Object Diagnostics.Process;$p.StartInfo=$info
 try{
  if(!$p.Start()){throw 'Launch failed'}
  $p.PriorityClass='BelowNormal'
  $out=$p.StandardOutput.ReadToEndAsync();$err=$p.StandardError.ReadToEndAsync()
  if(!$p.WaitForExit($timeout)){$p.Kill();throw "Test timed out: $name"}
  $text=$out.Result+"`r`n"+$err.Result+"`r`nExit="+$p.ExitCode
  $text|Set-Content (Join-Path $root ("logs\"+$name+'.txt'))
  Write-Output "$name Exit=$($p.ExitCode)"
  if($name -eq 'studio-native-live-ui' -and $text -notmatch 'PASS: live UI enable'){throw 'Missing explicit GUI completion marker'}; if($p.ExitCode -ne 0){Write-Output $text;throw "Test failed: $name"}
 }finally{$p.Dispose()}
}
try{
 RunTest 'soundcurrent-endpoint-identity-test.exe' '' 'endpoint-identity-unit'
 RunTest 'soundcurrent-windows-audio-smoke.exe' '--run-native' 'native-complete-transport' 110000
 RunTest 'soundcurrent-windows-audio-smoke.exe' '--run-native-mic' 'native-mic-silence'
 RunTest 'soundcurrent-studio.exe' '--windows-live-ui-test' 'studio-native-live-ui'
 RunTest 'soundcurrent-studio.exe' '--windows-live-manual-route-test' 'studio-native-manual-route'
 RunTest 'soundcurrent-windows-volume-smoke.exe' '--run-native' 'native-volume-guardian-recovery'
 'ALL INTEGRATION TESTS PASSED'|Set-Content (Join-Path $root 'logs\integration-result.txt')
}catch{$_|Out-String|Set-Content (Join-Path $root 'logs\integration-result.txt');throw}
finally{
 $saved.Restore()
 [AudioSafety]::State()|Set-Content (Join-Path $root 'logs\post-integration-audio-state.txt')
 [AudioSafety]::Capture([string[]]$ids)|ConvertTo-Json -Depth 5|Set-Content (Join-Path $root 'logs\integration-safety-after.json')
}
