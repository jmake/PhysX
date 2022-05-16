
template = open("/home/jovyan/work/PhysX/Babylonjs/babylon_template.html","r").read()
fieldsjs = open("/home/jovyan/work/PhysX/Babylonjs/BUILD/fields_new.js","r").read()

f = open("babylon.html","w")
f.write(template.replace("fieldsJS",fieldsjs))
f.close()
