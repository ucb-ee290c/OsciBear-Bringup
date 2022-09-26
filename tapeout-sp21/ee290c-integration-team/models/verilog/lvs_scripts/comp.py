a = set()
b = set()
with open("text_comp.txt", "r") as file:
    dst = a
    for line in file:
        line = line.strip()
        if not line: dst = b
        else:  
            for entry in line.split(" "):
                dst.add(entry.split("=")[0]) #.replace("<","[").replace(">","]"))

print(a - b)
print(b - a)

