docker build -t reproducibility -f Dockerfile_without_gui .
docker run -i -t --rm --name reproducibility reproducibility
