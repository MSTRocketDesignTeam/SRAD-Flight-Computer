```
python -m venv .venv/ 
.venv/Scripts/activate
python -m pip install -r requirements.txt
pyinstaller -y --console --clean --onefile --name SRAD.exe main.py 
```