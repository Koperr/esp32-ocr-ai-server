#include <stdlib.h>

int main() {
    const char *get_jpg_curl = "curl http://192.168.0.14/jpg --output ../../jpeg1.jpg";
    system(get_jpg_curl);

    // Przygotowanie zapytania cURL, które wysyła obrazek do serwera OCR
    const char *get_img_text_curl_command = "curl -X POST -F \"image=@../../jpeg1.jpg\" http://127.0.0.1:5000/ocr -o ../../ocr_output.txt";
    system(get_img_text_curl_command);

    const char *get_ai_respond_curl_command = "curl -X POST -F \"image=@../../img1.jpg\" http://127.0.0.1:5000/ocr";
    system(get_ai_respond_curl_command);

    return 0;
}