docker build -t reproducibility .
docker run -i -t --rm -p 8899:8899  --name reproducibility reproducibility
