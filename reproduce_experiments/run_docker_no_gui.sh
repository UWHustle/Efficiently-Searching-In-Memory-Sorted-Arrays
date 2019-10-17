docker build -t reproducibility -f Dockerfile_no_gui .
docker run -i -t --rm --name reproducibility reproducibility
