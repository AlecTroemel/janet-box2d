(declare-project
  :name "janet-box2d"
  :description "Janet bindings to Box2c (ie box2d version 3)"
  :url ""
  :author "Alec Troemel <alectroemel@hotmail.com>")

(rule "build/janetbox2d.so" ["CMakeLists.txt"]
      (do
	(os/mkdir "build")
	(os/cd "build")
	(os/execute ["cmake" ".."] :p)
	(assert
	  (zero?
	    (os/execute ["make"] :p)))))

(add-dep "build" "build/janetbox2d.so")
