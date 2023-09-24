# Basically a custom fork of the original ps4-libjbc repository by Sleirsgoevy, which will be maintained and updated by me

`ps4-libjbc` is a firmware-agnostic implementation of a sandbox escape for PS4 homebrew apps. It operates by traversing the process list up to PID 1 (init) and copying its prison and rdir into the calling process.<br>

In the future, more functionality (e.g., Mira-style "mount in sandbox") may be supported.
