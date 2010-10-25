import os
import subprocess

FERET_PATH = '/Users/alexanderkuk/workspace/mephi/gender_classifier/colorferet/colorferet/'
PARTITION_FILENAME = 'doc/partitions/fa.txt'
INFO_PATH = 'data/ground_truths/name_value/'
IMAGES_PATH = 'data/images'

RAWS_PATH = '/Users/alexanderkuk/workspace/mephi/gender_classifier/train_data/raw'
INFO_FILENAME = 'info.txt'

def GetGender(infoFilename):
    for line in open(infoFilename, 'r'):
        line = line.strip('\n')
        if line == 'gender=Male':
            return 'Male'
        if line == 'gender=Female':
            return 'Female'

def GetFeatures(infoFilename):
    features = ['', '', '', '']
    for line in open(infoFilename, 'r'):
        line = line.strip('\n')
        key, value = line.split('=')
        if key == 'left_eye_coordinates':
            features[1] = value
        elif key == 'right_eye_coordinates':
            features[0] = value
        elif key == 'nose_coordinates':
            features[2] = value
        elif key == 'mouth_coordinates':
            features[3] = value

    if features == ['', '', '', '']:
        return ''
    else:
        return "\t".join(features)
        
rawId = 0;
infoFile = open(os.path.join(RAWS_PATH, INFO_FILENAME), 'w')
dvd = 'dvd1'
for imageLocation in open(os.path.join(FERET_PATH, 'dvd1', PARTITION_FILENAME), 'r'):
    id, name = imageLocation.strip('\n').split(' ')
    if id == '00740':
        dvd = 'dvd2'
    name = name.strip('.ppm')
    
    infoRecord = str(rawId) + '.png' + '\t'
    features = GetFeatures(os.path.join(FERET_PATH, dvd, INFO_PATH, id, name + '.txt'))
    print("Processing " + name)
    if(features != ''):
        infoRecord = infoRecord + features + '\t'
        gender = GetGender(os.path.join(FERET_PATH, dvd, INFO_PATH, id, id + '.txt'))
        infoRecord = infoRecord + gender
        
        sourceBz2Filename = os.path.join(FERET_PATH, dvd, IMAGES_PATH, id, name + '.ppm.bz2')
        bz2Filename = os.path.join(RAWS_PATH, str(rawId) + '.ppm.bz2')
        ppmFilename = os.path.join(RAWS_PATH, str(rawId) + '.ppm')
        pngFilename = os.path.join(RAWS_PATH, str(rawId) + '.png')
        subprocess.call(['cp', sourceBz2Filename, bz2Filename])
        subprocess.call(['bzip2', '-d', bz2Filename])
        subprocess.call(['/usr/local/bin/convert', ppmFilename, pngFilename])
        subprocess.call(['rm', '-f', ppmFilename])

        rawId = rawId + 1
        infoFile.write(infoRecord + '\n')
    else:
        print("Has no description")
