# How to record display/matrix of oxocard

You can run a completly normal program, but be aware, that activating this recorder
may slow down the general card/experience.

## Compile so that your programm is being sent to possible recorders:
Put this in your Makefile:
```
CPPFLAGS += -DFB_LOG_OUTPUT
```

## Start server to record:
Simple python script which will _overwrite_ the output.json file. This file in
turn is compatible with the oxocard-simulator in bockly, which is basically the
replay-info.

```
python3 udp_server.py
```

# NOTES
This is all pretty hardcoded and only works withing the OXON-net. If it should
get used more general the IP-Part definitely needs adaptions. Also it may makes
sense to not send "rgb(r,g,b)", but all values shifted and packed within a 32bit
int. To reduce bandwith and such. Mabye there is a need to support timestamps.
Possible with the system-call millis()?


# LEGAL
This is basically forked from:
https://github.com/MalteJ/embedded-esp32-component-udp_logging/
