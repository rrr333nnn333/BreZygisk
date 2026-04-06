import com.android.build.gradle.LibraryExtension
import java.io.ByteArrayOutputStream

plugins {
    alias(libs.plugins.agp.lib) apply false
}

fun String.execute(currentWorkingDir: File = file("./")): String {
    val byteOut = ByteArrayOutputStream()
    project.exec {
        workingDir = currentWorkingDir
        commandLine = split("\\s".toRegex())
        standardOutput = byteOut
    }
    return String(byteOut.toByteArray()).trim()
}

val gitCommitCount = "git rev-list HEAD --count".execute().toInt()
val gitCommitHash = "git rev-parse --verify --short HEAD".execute()
val gitCommitsAhead = "git rev-list upstream/main..HEAD --count".execute().toInt()

val moduleId by extra("brezygisk")
val moduleName by extra("BreZygisk")
val verName by extra("v1.0.0")
val verCode by extra(gitCommitCount - gitCommitsAhead)
val verCode2 by extra(gitCommitsAhead)
val commitHash by extra(gitCommitHash)
val minAPatchVersion by extra(10655)
val minKsuVersion by extra(10940)
val minKsudVersion by extra(11425)
val maxKsuVersion by extra(20000)
val minMagiskVersion by extra(26402)

val androidMinSdkVersion by extra(26)
val androidTargetSdkVersion by extra(34)
val androidCompileSdkVersion by extra(34)
val androidBuildToolsVersion by extra("34.0.0")
val androidCompileNdkVersion by extra("29.0.13113456")
val androidSourceCompatibility by extra(JavaVersion.VERSION_11)
val androidTargetCompatibility by extra(JavaVersion.VERSION_11)

tasks.register("Delete", Delete::class) {
    delete(layout.buildDirectory.get())
}

fun Project.configureBaseExtension() {
    extensions.findByType(LibraryExtension::class)?.run {
        namespace = "com.performanc.org.rezygisk"
        compileSdk = androidCompileSdkVersion
        ndkVersion = androidCompileNdkVersion
        buildToolsVersion = androidBuildToolsVersion

        defaultConfig {
            minSdk = androidMinSdkVersion
        }

        lint {
            abortOnError = true
        }
    }
}

subprojects {
    plugins.withId("com.android.library") {
        configureBaseExtension()
    }
}
