# webserver

## Building

`cmake -DUVENT_PATH=<PATH/TO/UVENT> -DUVENT_BUILD_PATH=<PATH/TO/UVENT/BUILD> ..`
`make`

## Generating certificate
```bash
openssl genpkey -algorithm RSA -out server.key
openssl req -new -key server.key -out server.csr -subj '/C=RU/ST=Moscow/L=Moscow/O=tcpTestServer/OU=Unit Team/emailAddress=test@test.com/CN=test.com'
openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.pem
```
## To test Server it's possible to use CURL:
```bash
# HTTP2
curl --http2  --cacert /path/to/certificate/server.pem -X POST https://localhost:8080 -d '{"test":"test"}' # for test without debug info
curl --http2  --cacert /path/to/certificate/server.pem -X POST https://localhost:8080 -d '{"test":"test"}' -v # for test with debug info

# HTTP1
curl -0  --cacert /path/to/certificate/server.pem -X POST https://localhost:8080 -d '{"test":"test"}' # for test without debug info
curl -0  --cacert /path/to/certificate/server.pem -X POST https://localhost:8080 -d '{"test":"test"}' -v # for test with debug info 
```

## To test ServerTypes it's possible to use CURL:
```bash
# destination port should be the same as in toml file 
# ssl
curl --cacert /path/to/certificate/server.pem -v https://localhost:8111/hello
# no ssl
curl -v https://localhost:8111/hello

```

