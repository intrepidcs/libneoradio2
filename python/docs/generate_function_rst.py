import inspect
import neoradio2

if __name__ == "__main__":
    # Generate all the functions
    with open('functions.rst', 'w+') as f:
        f.write("============================================================\n")
        f.write("Function List\n")
        f.write("============================================================\n")
        
        
        f.write("\t.. autosummary::\n")
        #f.write("\t\t:toctree: _generate\n")
        #f.write("\n")

        for obj in inspect.getmembers(neoradio2):
            if "built-in method" in repr(obj[1]):
                print(obj)
                f.write("\t\t\tneoradio2.{}\n".format(str(obj[0])))
            else:
                print(obj)
                
        f.write("\n\t.. automodule:: neoradio2\n")
        f.write("\t\t:members:\n")
        f.write("\t\t:show-inheritance:\n")
        f.write("\t\t:undoc-members:\n")
                    

    # Generate all the functions
    with open('classes.rst', 'w+') as f:
        f.write("============================================================\n")
        f.write("Classes List\n")
        f.write("============================================================\n")
        
        
        f.write("\t.. autosummary::\n")
        #f.write("\t\t:toctree: _generate\n")
        #f.write("\n")

        for obj in inspect.getmembers(neoradio2):
            if "<class" in repr(obj[1]):
                print(obj)
                f.write("\t\t\tneoradio2.{}\n".format(str(obj[0])))
