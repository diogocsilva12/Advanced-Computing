# ==========================================================
# Generate Elsevier-Vancouver style with italic journals
# ==========================================================
# Run: python generate_vancouver_italic.py
# Output: elsevier-vancouver-italic.csl
# Compatible with Mendeley Cite & Zotero
# ==========================================================

xml = """<?xml version="1.0" encoding="UTF-8"?>
<style xmlns="http://purl.org/net/xbiblio/csl"
  version="1.0"
  default-locale="en-US"
  class="in-text"
  demote-non-dropping-particle="sort-only">
  <info>
    <title>Elsevier - Vancouver (Italic Journals)</title>
    <id>http://www.zotero.org/styles/elsevier-vancouver-italic</id>
    <link href="http://www.zotero.org/styles/elsevier-vancouver-italic" rel="self"/>
    <link href="http://www.zotero.org/styles/elsevier-vancouver" rel="template"/>
    <author>
      <name>ChatGPT Script</name>
    </author>
    <updated>2025-10-27T00:00:00+00:00</updated>
    <category citation-format="numeric"/>
    <category field="medicine"/>
    <summary>Elsevier Vancouver style with italic journals, DOI, and access date.</summary>
  </info>

  <!-- ===================== -->
  <!--  CITATION FORMAT      -->
  <!-- ===================== -->
  <citation collapse="citation-number" delimiter=",">
    <layout prefix="[" suffix="]" delimiter=",">
      <text variable="citation-number"/>
    </layout>
  </citation>

  <!-- ===================== -->
  <!--  BIBLIOGRAPHY FORMAT  -->
  <!-- ===================== -->
  <bibliography et-al-min="7" et-al-use-first="6" line-spacing="1" entry-spacing="0" hanging-indent="true">
    <layout>
      <text variable="citation-number" prefix="[" suffix="] "/>
      <text macro="author" suffix=". "/>
      <text variable="title" quotes="true" suffix=". "/>
      <text variable="container-title" font-style="italic" suffix=". "/>
      <group delimiter=", ">
        <text variable="volume" prefix="vol. "/>
        <text variable="issue" prefix="no. "/>
        <text variable="page" prefix="pp. "/>
      </group>
      <text variable="issued" prefix=", " suffix=". "/>
      <text variable="DOI" prefix="doi: " suffix=". "/>
      <text variable="URL" prefix="Available from: " suffix=". "/>
      <group delimiter=" ">
        <text term="accessed" text-case="capitalize-first" suffix=": "/>
        <date variable="accessed">
          <date-part name="year"/>
          <date-part name="month" form="short" prefix=" "/>
          <date-part name="day" prefix=" "/>
        </date>
      </group>
    </layout>
  </bibliography>

  <!-- ===================== -->
  <!--  MACROS               -->
  <!-- ===================== -->
  <macro name="author">
    <names variable="author">
      <name name-as-sort-order="all" and="text" delimiter=", " initialize-with=". "/>
      <label form="short" prefix=", "/>
    </names>
  </macro>
</style>
"""

with open("elsevier-vancouver-italic.csl", "w", encoding="utf-8") as f:
    f.write(xml)

print("✅ File 'elsevier-vancouver-italic.csl' created successfully!")
