### no projecto1

o 1o projecto que vai para a placa

1. ir ao menu de configuração
```
idf.py menuconfig
```
2. dar enable a CONFIG_PARTITION_TABLE_TWO_OTA

3. configure wi-fi, write ssid and pass of ap

4. configurar o Firmware Upgrade URL para
```
https://host-ip-address:port/projecto2.bin
```

### no projecto2

o que vai substituir o projecto1 via OTA e que tem de estar hosted no servidor

1. dar build e ir à pasta build e fazer
```
openssl req -x509 -newkey rsa:2048 -keyout ca_key.pem -out ca_cert.pem -days 365 -nodes
```

2. copiar os 2 certificados gerados e o projecto2.bin para a pasta OTA_server

3. ir a essa pasta e ligar o servidor
```
openssl s_server -WWW -key ca_key.pem -cert ca_cert.pem -port 8070
```




