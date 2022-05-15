
template = open("./GetGeometry/babylon_template.html","r").read()
fieldsjs = open("./fields_new.js","r").read()

f = open("babylon.html","w")
f.write(template.replace("fieldsJS",fieldsjs))
f.close()
