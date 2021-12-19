from http.server import HTTPServer, BaseHTTPRequestHandler

logpath = '/logstorage'


class LogPipeline(BaseHTTPRequestHandler):

    def do_GET(self):
        if self.path == '/':
            self.path = '/index.html'
        file_to_open = open(self.path[1:]).read()
        self.__set_response(200)
        self.wfile.write(bytes(file_to_open, 'utf-8'))

    def do_POST(self):
        try:
            content_len = int(self.headers.get('Content-Length'))
            post_body_bytes = self.rfile.read(content_len)
            post_body_string = str(post_body_bytes, "utf-8")
            self.__set_response(200)
            self.wfile.write(post_body_bytes)
        except:
            self.__set_response(200)
        self.write_to_file(post_body_string)

    def write_to_file(self, message):
        device = self.__find_device(message)
        logstring = self.__create_logstring(message)
        with open('{}/{}.log'.format(logpath, device), 'a') as f:
            f.write(logstring + '\n')

    def __set_response(self, response_code):
        self.send_response(response_code)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def __find_device(self, message):
        return message.split('&')[0].split('=')[1]

    def __create_logstring(self, message):
        items = message.split('&')[1:]
        logstring = ''
        for item in items:
            logstring += (item.split('=')[1] + " ")
        return logstring


httpd = HTTPServer(('0.0.0.0', 8081), LogPipeline)
httpd.serve_forever()
