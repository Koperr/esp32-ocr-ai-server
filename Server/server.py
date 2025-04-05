import os
import pytesseract
from flask import Flask, request, jsonify
from PIL import Image
import io

app = Flask(__name__)

# Jeśli tesseract nie jest w domyślnym katalogu, ustaw ścieżkę
#pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'  # Dla Windows, zmień ścieżkę na odpowiednią dla swojego systemu

@app.route('/ocr', methods=['POST'])
def ocr():
    # Sprawdź, czy plik jest w żądaniu
    if 'image' not in request.files:
        return jsonify({'error': 'Brak pliku obrazu w żądaniu'}), 400
    
    file = request.files['image']
    
    try:
        # Otwórz obrazek
        img = Image.open(file.stream)
        
        # Użyj tesseract do rozpoznania tekstu
        text = pytesseract.image_to_string(img)
        
        # Zwróć rozpoznany tekst
        return jsonify({'text': text}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True, port=5000)
