;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

;; (concat (cdr (project-current))

((nil . ((cmake-ide-project-dir . "~/repos/sample-hive")
         (cmake-ide-build-dir . "~/repos/sample-hive/build")
         (cmake-ide-cmake-opts . "-DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DPORTABLE=1 -DCMAKE_CXX_COMPILER='/usr/bin/g++'")
         ;; (projectile-project-root . "~/repos/sample-hive")
         (projectile-project-name . "SampleHive")
         (projectile-project-run-cmd . "~/repos/sample-hive/build/SampleHive")
         ;; (projectile-project-run-cmd . "/tmp/SampleHive/bin/SampleHive")
         (compile-command . "cd .. && ninja -C build install")
         (fill-column . 110)
         ;; Setup dap debugging template for the project
         (setq dap-debug-template-configurations
               '(("SampleHive::Core::Debug"
                  :type "gdb"
                  :request "launch"
                  :name "SampleHive::Core::Debug"
                  :target "${workspaceFolder}/build/SampleHive"
                  ;; :target "/tmp/SampleHive/bin/SampleHive"
                  :cwd "${workspaceFolder}"
                  :symbolLoadInfo "loadAll"
                  :additionalSOLibSearchPath "/tmp/SampleHive/bin/"))))))

;; ((c++-mode . ((dap-debug-template-configurations . '(("SampleHive::Core::Debug"
;;                                                       :type "gdb"
;;                                                       :request "launch"
;;                                                       :name "SampleHive::Core::Debug"
;;                                                       :target "${workspaceFolder}/build/SampleHive"
;;                                                       ;; :target "/tmp/SampleHive/bin/SampleHive"
;;                                                       :cwd "${workspaceFolder}"
;;                                                       :symbolLoadInfo "loadAll"
;;                                                       :additionalSOLibSearchPath "/tmp/SampleHive/bin/"))))))
