# libJBC
This is a custom version of sleirsgoevy's libjbc repo maintained by me.. <br>
It is a firmware-agnostic implementation of a sandbox escape for PS4 homebrew apps. <br>
It operates by traversing the process list up to PID 1 (init) and copying its prison and rdir into the calling process.<br>
<br><br>
In the future, more functionality (e.g., Mira-style "mount in sandbox") may be supported.
