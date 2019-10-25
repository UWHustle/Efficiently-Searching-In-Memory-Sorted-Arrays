docker build -t reproducibility . --no-cache=true
docker run -i -t --rm -p 8899:8899  --name reproducibility reproducibility
