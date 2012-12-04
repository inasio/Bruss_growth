from Crypto.Hash import SHA256
from bitstring import BitString

bit_file = BitString(filename="res091071/dump0")
hash_file = SHA256.new()
hash_file.update(bit_file.hex)
print hash_file.hexdigest()