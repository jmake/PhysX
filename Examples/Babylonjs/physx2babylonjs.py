
template = open("/home/Examples/Babylonjs/babylon_template.html","r").read()
fieldsjs = open("/home/Examples/Babylonjs/BUILD/fields_new.js","r").read()

f = open("babylon.html","w")
f.write(template.replace("fieldsJS",fieldsjs))
f.close()
