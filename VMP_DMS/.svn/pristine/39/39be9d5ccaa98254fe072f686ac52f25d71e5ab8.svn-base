import os
import os.path

def read_config(fname):
    ""
    res = {}
    section_name = 'DEFAULT'

    buf = open(fname).readlines()
    for i in range(len(buf)):
        t = buf[i]
        t = string.split(t, '#')[0]
        t = string.strip(t)

        if t:
            if t[0] == '[' and t[-1] == ']':
                section_name = string.strip(t[1:-1])
                if section_name:
                    res[section_name] = {}
            else:
                parts = string.split(t, ':')
                if len(parts) > 1:
                    res[section_name][string.strip(parts[0])] = string.strip(parts[1])

    return res


for root, dirs, files in os.walk('..'):
	print "root:", root
	print "dirs:", dirs
	print "files:", files
	print root, "consumes",
	print sum([os.path.getsize(os.path.join(root, name)) for name in files]),
	print "bytes in", len(files), "non-directory files"
	if 'CVS' in dirs:
		dirs.remove('CVS')  # don't visit CVS directories

