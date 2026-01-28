import groovy.io.FileType

import java.nio.file.Files
import java.nio.file.NoSuchFileException
import java.nio.file.Path

class FileHelper {
	static void copyDirectory(File dirFrom, File dirTo) {
		if (!dirFrom.exists()) throw new NoSuchFileException(dirFrom.toString())

		// creation the target dir
		if (!dirTo.exists()) dirTo.mkdir()

		// copying the daughter files
		dirFrom.eachFile(FileType.FILES) { File source ->
			File target = new File(dirTo, source.getName())
			target.bytes = source.bytes
		}

		// copying the daughter dirs - recursion
		dirFrom.eachFile(FileType.DIRECTORIES) { File source ->
			File target = new File(dirTo, source.getName())
			copyDirectory(source, target)
		}
	}

	static void deleteDir(File dir) {
		if (!dir.exists()) throw new NoSuchFileException(dir.toString())

		// Delete files first
		dir.eachFile(FileType.FILES) { it.delete() }

		// Recurse into subdirectories
		dir.eachFile(FileType.DIRECTORIES) { deleteDir(it) }

		// Finally delete the directory itself
		dir.delete()
	}

	static void deleteDirectoryIfExists(File dir) {
		if (dir.exists()) deleteDir(dir)
	}

	static void deleteFileIfExists(Path p) {
		if (Files.exists(p)) Files.delete(p)
	}

	static void deleteFileIfExists(File f) {
		deleteFileIfExists(f.toPath())
	}


//void deleteDirIfExists(File dir) {
//
//}
//
//void deleteFile(File f) {
//
//}
//
//void deleteFileIfExists(File f) {
//
//}

}
