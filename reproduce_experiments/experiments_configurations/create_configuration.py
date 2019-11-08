# Creates all the configuration files for the experiments presented in our
# paper.

import configurations_utils as utils


def fullConfiguration():
    return True


def figure2(tsvname):
    print("Configuring experiment : " + tsvname)
    tsv = tsvname + ".tsv"
    utils.rm_tsv(tsv)

    size = 5
    if fullConfiguration():
        size = 9

    for algorithm in ["bs", "sip", "is", "tip"]:
        utils.UaR_to_tsv(tsv, 10 ** size, algorithm, 8, 1)

    for algorithm in ["bs", "sip", "is", "tip"]:
        utils.fbids_to_tsv(tsv, algorithm, 8, 1)

    for algorithm in ["bs", "tip"]:
        utils.fal_to_tsv(tsv, algorithm, 8, 1, 1.05, 10 ** size)

    for algorithm in ["bs", "tip"]:
        utils.freq1_to_tsv(tsv, algorithm, 8, 1)


def figure5(tsvname):
    tsv = tsvname + ".tsv"
    utils.rm_tsv(tsv)

    for recordSize in [8, 32, 128]:
        for algorithm in ["sip", "bs", "tip", "is"]:
            utils.fbids_to_tsv(tsv, algorithm, recordSize, 1)


def figure6(tsvname):
    print("Configuring experiment : " + tsvname)
    tsv8 = tsvname + "_8.tsv"
    tsv32 = tsvname + "_32.tsv"
    tsv128 = tsvname + "_128.tsv"
    utils.rm_tsv(tsv8)
    utils.rm_tsv(tsv32)
    utils.rm_tsv(tsv128)

    datasetSizes = [3, 4, 5, 6, 7]
    if fullConfiguration():
        datasetSizes = [3, 4, 5, 6, 7, 8, 9]

    for algorithm in ["bs", "sip"]:
        for datasetSize in datasetSizes:
            utils.UaR_to_tsv(tsv8, 10 ** datasetSize, algorithm, 8, 1)
            utils.UaR_to_tsv(tsv32, 10 ** datasetSize, algorithm, 32, 1)
            if datasetSize == 9:
                continue
            utils.UaR_to_tsv(tsv128, 10 ** datasetSize, algorithm, 128, 1)


def figure7(tsvname):
    print("Configuring experiment : " + tsvname)
    tsv = tsvname + ".tsv"
    utils.rm_tsv(tsv)

    datasetSizes = [3, 4, 5, 6, 7]
    if fullConfiguration():
        datasetSizes = [3, 4, 5, 6, 7, 8, 9]

    for algorithm in ["bs", "sip"]:
        for datasetSize in datasetSizes:
            utils.UaR_to_tsv(tsv, 10 ** datasetSize, algorithm, 32, 1)


def figure8(tsvname):
    print("Configuring experiment : " + tsvname)
    tsv1 = tsvname + "_1.tsv"
    tsv2 = tsvname + "_2.tsv"
    tsv3 = tsvname + "_3.tsv"
    tsv4 = tsvname + "_4.tsv"

    utils.rm_tsv(tsv1)
    utils.rm_tsv(tsv2)
    utils.rm_tsv(tsv3)
    utils.rm_tsv(tsv4)

    gapShapes = [0.7, 0.9, 0.99, 0.9999]
    tsvNames = [tsv1, tsv2, tsv3, tsv4]

    datasetSizes = [4, 5, 6, 7]
    if fullConfiguration():
        datasetSizes = [4, 5, 6, 7, 8]

    recordSizes = [8, 32, 128]
    algorithms = ["isseq", "sip"]

    for i in range(0, len(gapShapes)):
        for recordSize in recordSizes:
            for datasetSize in datasetSizes:
                    for algorithm in algorithms:
                        utils.gap_to_tsv(tsvNames[i], algorithm,
                                         recordSize, 1, 42, gapShapes[i],
                                         10 ** datasetSize)

def figure9(tsvname):
    print("Configuring experiment : " + tsvname)
    tsvfal = tsvname + "_fal.tsv"
    tsvcfal = tsvname + "_cfal.tsv"
    utils.rm_tsv(tsvfal)
    utils.rm_tsv(tsvcfal)

    datasetSizes = [3, 4, 5, 6, 7]
    if fullConfiguration():
        datasetSizes = [3, 4, 5, 6, 7, 8, 9]

    shapes = [0.5, 1.05, 1.25, 1.5]
    for algorithm in ["bs", "tip"]:
        for datasetSize in datasetSizes:
            for shape in shapes:
                utils.fal_to_tsv(tsvfal, algorithm, 8, 1, shape,
                                 10 ** datasetSize)

    for algorithm in ["bs", "tip"]:
        for datasetSize in datasetSizes:
            for shape in shapes:
                utils.cfal_to_tsv(tsvcfal, algorithm, 8, 1, shape,
                                  10 ** datasetSize)


def figure10(tsvname):
    print("Configuring experiment : " + tsvname)
    tsv = tsvname + ".tsv"
    utils.rm_tsv(tsv)

    for algorithm in ["sip", "isseq"]:
        utils.fbids1k_to_tsv(tsv, algorithm, 8, 1)
        utils.fbids10k_to_tsv(tsv, algorithm, 8, 1)
        utils.fbids100k_to_tsv(tsv, algorithm, 8, 1)
        utils.fbids_to_tsv(tsv, algorithm, 8, 1)


def figure11(tsvname):
    print("Configuring experiment : " + tsvname)
    tsv = tsvname + ".tsv"
    utils.rm_tsv(tsv)

    for algorithm in ["tip", "bs"]:
        for recordSize in [8, 32, 128]:
            utils.freq1_to_tsv(tsv, algorithm, recordSize, 1)

    for algorithm in ["tip", "bs"]:
        for recordSize in [8, 32, 128]:
            utils.freq2_to_tsv(tsv, algorithm, recordSize, 1)

def Section56_SIP_UAR(tsvname):
    print("Configuring experiment : " + tsvname)
    tsv = tsvname + ".tsv"
    utils.rm_tsv(tsv)

    datasetSizes = [3, 4, 5, 6, 7]
    if fullConfiguration():
        datasetSizes = [3, 4, 5, 6, 7, 8, 9]

    for algorithm in ["b-eyt-p", "b-eyt"]:
        for datasetSize in datasetSizes:
            utils.UaR_to_tsv(tsv, 10 ** datasetSize, algorithm, 8, 1)

def Section56_SIP_FB(tsvname):
    print("Configuring experiment : " + tsvname)
    tsv = tsvname + ".tsv"
    utils.rm_tsv(tsv)

    recordSizes = [8, 32, 128]

    for algorithm in ["bs", "sip", "b-eyt-p", "b-eyt"]:
        for recordSize in recordSizes:
            utils.fbids_to_tsv(tsv, algorithm, recordSize, 1)


def Section56_TIP(tsvname):
    print("Configuring experiment : " + tsvname)
    tsvfal = tsvname + "_fal.tsv"
    tsvcfal = tsvname + "_cfal.tsv"
    utils.rm_tsv(tsvfal)
    utils.rm_tsv(tsvcfal)

    datasetSizes = [3, 4, 5, 6, 7]
    if fullConfiguration():
        datasetSizes = [3, 4, 5, 6, 7, 8, 9]

    shapes = [0.5, 1.05, 1.25, 1.5]
    for algorithm in ["b-eyt-p", "b-eyt"]:
        for datasetSize in datasetSizes:
            for shape in shapes:
                utils.fal_to_tsv(tsvfal, algorithm, 8, 1, shape,
                                 10 ** datasetSize)

    for algorithm in ["b-eyt-p", "b-eyt"]:
        for datasetSize in datasetSizes:
            for shape in shapes:
                utils.cfal_to_tsv(tsvcfal, algorithm, 8, 1, shape,
                                  10 ** datasetSize)

def figure12(tsvname):
    print("Configuring experiment : " + tsvname)
    tsv = tsvname + ".tsv"
    utils.rm_tsv(tsv)

    datasetSizes = [3, 4, 5, 6, 7, 8]

    for datasetSize in datasetSizes:
            utils.UaR_to_tsv(tsv, 10 ** datasetSize, "sip_metadata", 8, 1)


    tsv2 = tsvname + "_times.tsv"
    utils.rm_tsv(tsv2)
    for datasetSize in datasetSizes:
        utils.UaR_to_tsv(tsv2, 10 ** datasetSize, "sip", 8, 1)

################################################


figure2("fig2")
figure5("fig5")
figure6("fig6")
figure7("fig7")
figure8("fig8")
figure9("fig9")
figure10("fig10")
figure11("fig11")
figure12("fig12")
Section56_SIP_UAR("section56_SIP_UAR")
Section56_SIP_FB("section56_SIP_FB")
Section56_TIP("section56_TIP")
