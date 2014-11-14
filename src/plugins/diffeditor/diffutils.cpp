** conditions see http://www.qt.io/licensing.  For further information
** use the contact form at http://www.qt.io/contact-us.
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
    Differ::diffBetweenEqualities(leftDiffList,
                                  rightDiffList,
                                  &outputLeftDiffList,
                                  &outputRightDiffList);
    }
    const QRegExp leftFileRegExp(QLatin1String("^-{3} ")                 // "--- "
                                 + leftFileName                          // "a/fileName" or "/dev/null"
                                 + QLatin1String("(?:\\t[^\\n]*)*\\n")); // optionally followed by: \t anything \t anything ...)
    const QRegExp rightFileRegExp(QLatin1String("^\\+{3} ")               // "+++ "
                                  + rightFileName                         // "b/fileName" or "/dev/null"
                                  + QLatin1String("(?:\\t[^\\n]*)*\\n")); // optionally followed by: \t anything \t anything ...)
    const QRegExp binaryRegExp(QLatin1String("^Binary files ")
                               + leftFileName
                               + QLatin1String(" and ")
                               + rightFileName
                               + QLatin1String(" differ$"));

    // empty or followed either by leftFileRegExp or by binaryRegExp
    if (patch.isEmpty() && (fileData.fileOperation == FileData::NewFile
                         || fileData.fileOperation == FileData::DeleteFile)) {
        readOk = true;
    } else if (leftFileRegExp.indexIn(patch) == 0) {
        patch.remove(0, leftFileRegExp.matchedLength());
        // followed by rightFileRegExp
        if (rightFileRegExp.indexIn(patch) == 0) {
            patch.remove(0, rightFileRegExp.matchedLength());
            fileData.chunks = readChunks(patch,
                                         &fileData.lastChunkAtTheEndOfFile,
                                         &readOk);
    } else if (binaryRegExp.indexIn(patch) == 0) {
        readOk = true;
        fileData.binaryFiles = true;
static QList<FileData> readGitPatch(const QString &patch, bool *ok)
QList<FileData> DiffUtils::readPatch(const QString &patch, bool *ok)
    fileDataList = readGitPatch(croppedPatch, &readOk);
        fileDataList = readDiffPatch(croppedPatch, &readOk);