source lamachine-activate
# work around for https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=934731
sudo chmod -f 600 /run/uwsgi-emperor.pid
lamachine-stop-webserver
sudo killall -q uwsgi

# sudo for moving data-folder required
sudo lamachine-start-webserver

# keep it open
bash
