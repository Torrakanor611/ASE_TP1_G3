## raw instalation/setup of ESP-IDF for linux:

[https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html)

### Step 1 and Step 2 as is

### Step 3

```
cd ~/esp/esp-idf
./install.sh esp32
```

### Step 4

add this alias on .bashrc

```
alias get_idf='. $HOME/esp/esp-idf/export.sh'
```

Now you can run get_idf to set up or refresh the esp-idf environment in any terminal session.

### Step 5
to build a project
```
idf.py build
```

### Step 6
to flash it to esp32 device
```
idf.py -p (PORT) flash
```

to discover the port:
```
l -ls /dev/tty*
```
do 1st time with board connected and 2nd time disconnected and spot the one /dev/tty* missing

probably it will be /dev/ttyUSBx
### Step 7
start monitor mode
```
idf.py -p (PORT) monitor
```
**Note:** The option flash automatically builds and flashes the project, so to build, flash and monitor do:
```
idf.py -p /dev/ttyUSB0 flash monitor
```
## Possible issues
> Failed to open port /dev/ttyUSB0

```
sudo usermod -a -G dialout $USER
```

> PermissionError: [Errno 13] Permission denied: '/dev/ttyUSB0'

```
sudo chmod a+rw /dev/ttyUSB0
```
> A fatal error occurred: Failed to connect to ESP32: Wrong boot mode detected (0x13)! The chip needs to be in download mode.

Hold down boot button while flashing

<https://randomnerdtutorials.com/solved-failed-to-connect-to-esp32-timed-out-waiting-for-packet-header/>

### to leave monitor mode
docs say use **ctrl+]** but european keyboard layout so use **ctrl+alt+5**
