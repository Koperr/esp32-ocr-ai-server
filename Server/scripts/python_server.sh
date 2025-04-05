python3 -m venv venv
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"
cd ..

# Aktywacja środowiska wirtualnego
. venv/bin/activate  # użyj kropki, bo source nie działa w sh

# Instalacja pakietów
pip install pytesseract flask

cd "$DIR"
cd ..
python3 server.py