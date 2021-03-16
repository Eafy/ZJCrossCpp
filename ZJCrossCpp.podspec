Pod::Spec.new do |s|
  s.name         = "ZJCrossCpp"
  s.version      = "1.0.0"
  s.summary      = "C++ cross-platform tool set and basic framework. "
  s.description  = "C++ cross-platform tool set and basic framework. "

  s.homepage     = "https://github.com/Eafy/ZJCrossCpp"
  s.license      = { :type => "MIT" }
  s.author       = "Eafy"
  s.platform     = :ios, "9.0"
  s.requires_arc = true
  s.pod_target_xcconfig = { 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'arm64' }
  
  s.source       = { :git => "https://github.com/Eafy/ZJCrossCpp.git", :tag => "v#{s.version}" }
  s.vendored_libraries = 'Libs/iOS/**/*.a'
  s.resources = 'Libs/iOS/**/*.a'
  s.libraries = 'c++'
  s.dependency 'ZJLog', '1.1.6'

  s.source_files  = "Libs/iOS/**/*.{h,hpp}", "Cpp/**/*.{h,hpp,c,cpp}", "Cpp/**/*.{h,hpp,c,cpp}"
  s.public_header_files = "Libs/iOS/**/*.{h,hpp}", "Cpp/**/*.{h,hpp}"
  
end

#校验指令
#pod lib lint ZJCrossCpp.podspec --verbose --allow-warnings --use-libraries
#打包指令
#pod package ZJCrossCpp.podspec --force --no-mangle --exclude-deps --verbose
#发布命令
#pod trunk push ZJCrossCpp.podspec --verbose --allow-warnings --use-libraries
