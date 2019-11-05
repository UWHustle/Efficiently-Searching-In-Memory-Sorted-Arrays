# Creates all the configuration files for the experiments presented in our
# paper.

import configurations_utils as utils


def fullConfiguration():
    return True


def figure2(tsvname):
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
    tsv = tsvname + ".tsv"
    utils.rm_tsv(tsv)

    datasetSizes = [3, 4, 5, 6, 7]
    if fullConfiguration():
        datasetSizes = [3, 4, 5, 6, 7, 8, 9]

    for algorithm in ["bs", "sip"]:
        for datasetSize in datasetSizes:
            utils.UaR_to_tsv(tsv, 10 ** datasetSize, algorithm, 32, 1)


def figure8(tsvname):
    tsv1 = tsvname + "_1.tsv"
    tsv1 = tsvname + "_2.tsv"
    tsv1 = tsvname + "_3.tsv"
    tsv1 = tsvname + "_4.tsv"
    tsv1 = tsvname + "_5.tsv"

    utils.rm_tsv(ts1)
    utils.rm_tsv(tsv2)
    utils.rm_tsv(tsv3)
    utils.rm_tsv(tsv4)
    utils.rm_tsv(tsv5)

    gapShapes = [0.7, 0.9, 0.99, 0.99999]
    datasetSizes = [3, 5, 6, 7]
    recordSizes = [8,32,128]
    algorithms = ["sip", "isseq"]

    for recordSize in recordSizes:
        for datasetSize in datasetSizes:
                for algorithm in algorithms:
                    utils.gap_to_tsv(tsv1, algorithm, recordSize, 1, 42, 0.7,
                                 10 ** datasetSize)

    for recordSize in recordSizes:
        for datasetSize in datasetSizes:
            for algorithm in algorithms:
                utils.gap_to_tsv(tsv2, algorithm, recordSize, 1, 42, 0.8,
                                 10 ** datasetSize)

    for recordSize in recordSizes:
        for datasetSize in datasetSizes:
            for algorithm in algorithms:
                utils.gap_to_tsv(tsv3, algorithm, recordSize, 1, 42, 0.9,
                                 10 ** datasetSize)

    for recordSize in recordSizes:
        for datasetSize in datasetSizes:
            for algorithm in algorithms:
                utils.gap_to_tsv(tsv4, algorithm, recordSize, 1, 42, 0.99,
                                 10 ** datasetSize)

    for recordSize in recordSizes:
        for datasetSize in datasetSizes:
            for algorithm in algorithms:
                utils.gap_to_tsv(tsv5, algorithm, recordSize, 1, 42, 0.9999,
                                 10 ** datasetSize)


# def figure8(tsvname):
#     tsv8 = tsvname + "_8.tsv"
#     tsv32 = tsvname + "_32.tsv"
#     tsv128 = tsvname + "_128.tsv"
#     utils.rm_tsv(tsv8)
#     utils.rm_tsv(tsv32)
#     utils.rm_tsv(tsv128)
#
#     datasetSizes = [3, 5, 6, 7]
#     for algorithm in ["sip", "isseq"]:
#         for gapShape in [0.7, 0.9, 0.99, 0.99999]:
#             for datasetSize in datasetSizes:
#                 utils.gap_to_tsv(tsv8, algorithm, 8, 1, 47, gapShape,
#                                  10 ** datasetSize)
#
#     for algorithm in ["sip", "isseq"]:
#         for gapShape in [0.7, 0.9, 0.99, 0.99999]:
#             for datasetSize in datasetSizes:
#                 utils.gap_to_tsv(tsv32, algorithm, 32, 1, 47, gapShape,
#                                  10 ** datasetSize)
#
#     for algorithm in ["sip", "isseq"]:
#         for gapShape in [0.7, 0.9, 0.99, 0.9999]:
#             for datasetSize in datasetSizes:
#                 utils.gap_to_tsv(tsv128, algorithm, 128, 1, 47, gapShape,
#                                  10 ** datasetSize)


def figure9(tsvname):
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
    tsv = tsvname + ".tsv"
    utils.rm_tsv(tsv)

    for algorithm in ["sip", "isseq"]:
        utils.fbids1k_to_tsv(tsv, algorithm, 8, 1)
        utils.fbids10k_to_tsv(tsv, algorithm, 8, 1)
        utils.fbids100k_to_tsv(tsv, algorithm, 8, 1)
        utils.fbids_to_tsv(tsv, algorithm, 8, 1)


def figure11(tsvname):
    tsv = tsvname + ".tsv"
    utils.rm_tsv(tsv)

    for algorithm in ["tip", "bs"]:
        for recordSize in [8, 32, 128]:
            utils.freq1_to_tsv(tsv, algorithm, recordSize, 1)

    for algorithm in ["tip", "bs"]:
        for recordSize in [8, 32, 128]:
            utils.freq2_to_tsv(tsv, algorithm, recordSize, 1)


################################################


figure2("fig2")
figure5("fig5")
figure6("fig6")
figure7("fig7")
figure8("fig8")
figure9("fig9")
figure10("fig10")
figure11("fig11")
