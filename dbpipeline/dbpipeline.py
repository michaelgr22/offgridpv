from requests_pkcs12 import get

from credentials import AWS_ENDPOINT, PKCS12_PASSWORD, PORT

PKS12 = 'dbpipeline_credentials.p12'

url = 'https://{}:{}/things/offgridpvtest/shadow'.format(AWS_ENDPOINT, PORT)

response = get(url, pkcs12_filename=PKS12,
               pkcs12_password=PKCS12_PASSWORD)

print(response.text)
