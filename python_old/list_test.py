import os
import shutil

def touch(path) :
    with open(path, 'a') :
        os.utime(path, None)

def get_tiff_list(path, level=1) :
    tiff_list = []

    # strip the last separator and count it
    path = path.rstrip(os.path.sep)
    sep_cnt = path.count(os.path.sep)

    for (root, dirs, files) in os.walk(path) :
        # count separators in current level
        this_sep_cnt = root.count(os.path.sep)
        if ((sep_cnt+level) <= this_sep_cnt) :
            del dirs[:]
        else :
            for file in files :
                if file.endswith('.tif') or file.endswith('.tiff') :
                    tiff_list.append(os.path.join(root, file))

    return tiff_list

# Create directory for temporary file.
basedir = 'C:\\test_dir';
if os.path.exists(basedir) :
    shutil.rmtree(basedir)
os.makedirs(basedir)

# Generate dummy files.
for i in range(1, 1000+1) :
    touch(os.path.join(basedir, str(i) + '.tif'))

# List the files.
file_list = get_tiff_list(basedir)
print(file_list)
