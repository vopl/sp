base=http://www.repository.voxforge1.org/downloads/Russian/Trunk/Audio/Original/48kHz_16bit

mkdir -p tgz
mkdir -p uncompressed

echo base=${base} > items.sh

wget -O index.html ${base}/

cat index.html | \
    grep "a href=" | \
    grep tgz | \
    awk -F\" '{print "wget -P tgz -c ${base}/"$6" || exit 1"; print "tar -C uncompressed -xzf tgz/"$6" || exit 1"}' >> items.sh

chmod +x items.sh || exit 1

./items.sh || exit 1

echo all done
