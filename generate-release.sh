folder=$(echo $@ | gawk 'match($0, /(.*): release (.*)/, a) { print(a[1]) }')
version=$(echo $@ | gawk 'match($0, /(.*): release (.*)/, a) { print(a[2]) }')
cd $folder
mkdir -p release/
zip -r release/$version.zip $(basename $folder)/
echo $version >> version.txt
