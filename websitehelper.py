import shutil
import os
import glob

def make_html_allplots(plotdir,version,sample,subdir):
    f = open('/web/sites/m/microboone-validation.fnal.gov/htdocs/'+version+'_'+sample+'_'+subdir+'.html','w')

    # FNAL webpage boilerplate
    f.write('<!--#set var="META_TITLE" value="uBooNE validation | '+version+'"-->\n')
    f.write('<!--#set var="PAGE_HEADER" value="MicroBooNE Validation Plots"-->\n')
    f.write('<!--#set var="BODY_CLASSES" value="section-default"-->\n')
    f.write('\n')
    f.write('<!--#include virtual="/fnalincludes/page-top.html"-->\n')
    f.write('<!--#include virtual="/localincludes/nav-left.html"-->\n')
    f.write('<!--#include virtual="/fnalincludes/top-close.html"-->\n')
    f.write('<!--//************ DO NOT REMOVE CODE ABOVE ***********//-->\n')
    f.write('\n')

    f.write('<h2 class="page-title">uboonecode '+version+': '+sample+', '+subdir+'</h2>\n')

    # Now put in all the plots in the directory
    plots = glob.glob(plotdir+'/*.jpg') + glob.glob(plotdir+'/*.png') + glob.glob(plotdir+'/*.gif') + glob.glob(plotdir+'/*.pdf')
    for plot in plots:
        plotname = os.path.basename(plot)
        #f.write('<a '+plotname+'><img src="'+plot+' alt='+plotname+' style="border: none; width: 400px;"/></a><br /><br />\n')
        f.write('<figure>\n <img src="'+plot[len('/web/sites/m/microboone-validation.fnal.gov/'):]+'" alt="'+plotname+'" style="border: none; width: 400px;" />\n <figcaption>\n '+plotname+'\n </figcaption>\n </figure>\n')
        f.write('\n')

    #f.write('  <?php\n')
    #f.write('    foreach (glob("'+plotdir+'/*") as $file){\n')
    #f.write('    $filename = basename($file);\n')
    #f.write('    echo "<a $filename><img src=\\"$file\\" alt=$filename style=\\"border: none; width: 300px; \\"></a><br /><br />";\n')
    #f.write('    }\n')
    #f.write('    ?>\n')

    # More FNAL webpage boilerplate
    f.write('\n')
    f.write('<!--//************* DO NOT REMOVE CODE BELOW ***********//-->\n')
    f.write('\n')
    f.write('<!--//========== Last Modifed ==========//-->\n')
    f.write('<div class="last-modified">\n')
    f.write('    <div class="last-modified-inner">\n')
    f.write('    	<ul>\n')
    f.write('        	<li class="first title">Last modified</li>\n')
    f.write('			<li class="date"><!-- #BeginDate format:Am3 --><!--#config timefmt="%m/%d/%Y" --><!--#echo var="LAST_MODIFIED"--><!-- #EndDate --></li>\n')
    f.write('        	<li class="last email"><a href="http://www.fnal.gov/pub/contact/email.html">email Fermilab</a></li>\n')
    f.write('        </ul>\n')
    f.write('    </div><!-- /.last-modified-inner -->\n')
    f.write('</div><!-- /.last-modified -->\n')
    f.write('<!--//========== END: Last Modifed ==========//-->\n')
    f.write('\n')
    f.write('<!--#include virtual="/fnalincludes/page-bottom.html"-->\n')

#-------------------------------------------------------------------------------#
#-------------------------------------------------------------------------------#
#-------------------------------------------------------------------------------#

def make_version_html(version, samples_list, subdirs_list):
    f = open('/web/sites/m/microboone-validation.fnal.gov/htdocs/'+version+'.html','w')

    # FNAL webpage boilerplate
    f.write('<!--#set var="META_TITLE" value="uBooNE validation | '+version+'"-->\n')
    f.write('<!--#set var="PAGE_HEADER" value="MicroBooNE Validation Plots"-->\n')
    f.write('<!--#set var="BODY_CLASSES" value="section-default"-->\n')
    f.write('\n')
    f.write('<!--#include virtual="/fnalincludes/page-top.html"-->\n')
    f.write('<!--#include virtual="/localincludes/nav-left.html"-->\n')
    f.write('<!--#include virtual="/fnalincludes/top-close.html"-->\n')
    f.write('<!--//************ DO NOT REMOVE CODE ABOVE ***********//-->\n')
    f.write('\n')

    # This is the bit where we actually write the stuff we want (a sample header for each sample and links to version_sample_subdir.php for each sample subdirectory)
    f.write('<h2 class="page-title">uboonecode '+version+'</h2>\n')
    f.write('\n')
    for i_sample in range(0,len(samples_list)):
        sample = samples_list[i_sample]
        f.write('<h3>'+sample+'</h3>\n')
        f.write('\n')
        for i_subdir in range(0,len(subdirs_list[i_sample])):
            subdir = subdirs_list[i_sample][i_subdir]
            f.write('<p><a href="'+version+'_'+sample+'_'+subdir+'.html'+'">'+subdir+'</a></p>\n')
            f.write('\n')
    f.write('\n')
    f.write('\n')


    # More FNAL webpage boilerplate
    f.write('\n')
    f.write('<!--//************* DO NOT REMOVE CODE BELOW ***********//-->\n')
    f.write('\n')
    f.write('<!--//========== Last Modifed ==========//-->\n')
    f.write('<div class="last-modified">\n')
    f.write('    <div class="last-modified-inner">\n')
    f.write('    	<ul>\n')
    f.write('        	<li class="first title">Last modified</li>\n')
    f.write('			<li class="date"><!-- #BeginDate format:Am3 --><!--#config timefmt="%m/%d/%Y" --><!--#echo var="LAST_MODIFIED"--><!-- #EndDate --></li>\n')
    f.write('        	<li class="last email"><a href="http://www.fnal.gov/pub/contact/email.html">email Fermilab</a></li>\n')
    f.write('        </ul>\n')
    f.write('    </div><!-- /.last-modified-inner -->\n')
    f.write('</div><!-- /.last-modified -->\n')
    f.write('<!--//========== END: Last Modifed ==========//-->\n')
    f.write('\n')
    f.write('<!--#include virtual="/fnalincludes/page-bottom.html"-->\n')


#-------------------------------------------------------------------------------#
#-------------------------------------------------------------------------------#
#-------------------------------------------------------------------------------#

def add_to_nav_left(version):

    # Check if this link already exists and don't add it if it does
    if '"'+version+'.html"' in open('/web/sites/m/microboone-validation.fnal.gov/htdocs/localincludes/nav-left.html').read():
        print 'Found "'+version+'.html" in htdocs/localincludes/nav-left.html. Since this link already exists, will not add a duplicate.'
        return
    else:
        # Make a copy of nav-left.html so we can read it in
        shutil.move('/web/sites/m/microboone-validation.fnal.gov/htdocs/localincludes/nav-left.html','/web/sites/m/microboone-validation.fnal.gov/htdocs/localincludes/nav-left.html~')

        # Make a new nav-left. Copy lines from the old one, but add a new entry for the given version
        destination = open('/web/sites/m/microboone-validation.fnal.gov/htdocs/localincludes/nav-left.html','w')
        source = open('/web/sites/m/microboone-validation.fnal.gov/htdocs/localincludes/nav-left.html~','r')

        for line in source:
            destination.write(line)
            if '<ul>' in line:
                destination.write('\t<li><a href="'+version+'.html">'+version+'</a></li>\n')
        source.close()
        destination.close()
        os.remove('/web/sites/m/microboone-validation.fnal.gov/htdocs/localincludes/nav-left.html~')
