#!/usr/bin/env python3


import os
from http.server import HTTPServer, SimpleHTTPRequestHandler
from urllib.parse import unquote
from pathlib import Path
import ssl

BASE_DIR = Path(os.getenv("BASE_DIR", "./shared")).resolve()
BASE_DIR.mkdir(parents=True, exist_ok=True)


class NoListingHandler(SimpleHTTPRequestHandler):
    def translate_path(self, path):
        # Декодируем %20 и т.п.
        path = unquote(path)
        # убираем ведущие /
        path = path.lstrip("/")
        # абсолютный путь
        abs_path = (BASE_DIR / path).resolve()
        try:
            abs_path.relative_to(BASE_DIR)
        except ValueError:
            return str(BASE_DIR / "__404__")  # за пределы -> 404
        return str(abs_path)

    def list_directory(self, path):
        # отключаем листинг каталогов
        self.send_error(404, "http://natribu.org/")
        return None


if __name__ == "__main__":
    host = os.getenv("HOST", "0.0.0.0")
    port = int(os.getenv("PORT", "8000"))
    httpd = HTTPServer((host, port), NoListingHandler)

    # SSL контекст
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    ssl_context.load_cert_chain(certfile="cert.pem", keyfile="key.pem")  # свои сертификаты

    # Оборачиваем HTTP-сервер в SSL
    #httpd.socket = ssl_context.wrap_socket(httpd.socket, server_side=True)

    print(f"Serving HTTPS files from {BASE_DIR} on {host}:{port}")
    httpd.serve_forever()

