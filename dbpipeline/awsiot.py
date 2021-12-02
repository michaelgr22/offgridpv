import json
from requests_pkcs12 import get


class AwsIot:
    def __init__(self, aws_endpoint, aws_port, pkcs12_password):
        self.aws_endpoint = aws_endpoint
        self.aws_port = aws_port
        self.pkcs12 = 'dbpipeline_credentials.p12'
        self.pkcs12_password = pkcs12_password

    def get_thing_shadow(self, thing):
        url = 'https://{}:{}/things/{}/shadow'.format(
            self.aws_endpoint, self.aws_port, thing)

        return self.__response_to_json(get(url, pkcs12_filename=self.pkcs12,
                                           pkcs12_password=self.pkcs12_password))

    def __response_to_json(self, response):
        return json.loads(response.text.replace("'", "\""))
