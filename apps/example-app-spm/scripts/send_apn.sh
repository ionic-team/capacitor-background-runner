TEAM_ID=9YN2HU59K8
TOKEN_KEY_FILE_NAME=/Users/jpender/Downloads/AuthKey_U939TFQ2G4.p8
AUTH_KEY_ID=U939TFQ2G4
TOPIC=io.ionic.backgroundrunner.example
DEVICE_TOKEN=c04b70d63712aebd7e6c6d96e5e820cff9c9671d72b0ba04ccc03ba8790cf025
APNS_HOST_NAME=api.sandbox.push.apple.com

openssl s_client -connect "${APNS_HOST_NAME}":443

JWT_ISSUE_TIME=$(date +%s)
JWT_HEADER=$(printf '{ "alg": "ES256", "kid": "%s" }' "${AUTH_KEY_ID}" | openssl base64 -e -A | tr -- '+/' '-_' | tr -d =)
JWT_CLAIMS=$(printf '{ "iss": "%s", "iat": %d }' "${TEAM_ID}" "${JWT_ISSUE_TIME}" | openssl base64 -e -A | tr -- '+/' '-_' | tr -d =)
JWT_HEADER_CLAIMS="${JWT_HEADER}.${JWT_CLAIMS}"
JWT_SIGNED_HEADER_CLAIMS=$(printf "${JWT_HEADER_CLAIMS}" | openssl dgst -binary -sha256 -sign "${TOKEN_KEY_FILE_NAME}" | openssl base64 -e -A | tr -- '+/' '-_' | tr -d =)
AUTHENTICATION_TOKEN="${JWT_HEADER}.${JWT_CLAIMS}.${JWT_SIGNED_HEADER_CLAIMS}"

curl -v --header "apns-topic: $TOPIC" --header "apns-push-type: background" --header "apns-priority: 5" --header "authorization: bearer $AUTHENTICATION_TOKEN" --data '{"aps":{"content-available":1},"acme1":"bar","acme2":42}' --http2 https://${APNS_HOST_NAME}/3/device/${DEVICE_TOKEN}
