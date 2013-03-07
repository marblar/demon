<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:msxsl="urn:schemas-microsoft-com:xslt"
                exclude-result-prefixes="msxsl">

  <xsl:output method="xml"
              indent="yes"/>

  <xsl:template match="TestResult">
    <test-results>
      <xsl:attribute name="total">
        <xsl:value-of select="sum(./TestSuite/@test_cases_passed) + sum(./TestSuite/@test_cases_failed) + sum(./TestSuite/@test_cases_skipped) + sum(./TestSuite/@test_cases_aborted)"/>
      </xsl:attribute>
      <xsl:attribute name="failures">
        <xsl:value-of select="sum(./TestSuite/@test_cases_failed) + sum(./TestSuite/@test_cases_aborted)"/>
      </xsl:attribute>
      <xsl:attribute name="skipped">
        <xsl:value-of select="sum(./TestSuite/@test_cases_skipped)"/>
      </xsl:attribute>
      <xsl:attribute name="not-run">
        <xsl:value-of select="sum(./TestSuite/@test_cases_skipped)"/>
      </xsl:attribute>
      <xsl:call-template name="testSuite" />
    </test-results>
  </xsl:template>

  <xsl:template name="testSuite">
    <xsl:for-each select="TestSuite">
      <test-suite>
        <xsl:call-template name="testAttributes" />
        <results>
          <xsl:call-template name="testSuite" />
          <xsl:for-each select="TestCase">
            <test-case>
              <xsl:call-template name="testAttributes" />
            </test-case>
          </xsl:for-each>
        </results>
      </test-suite>
    </xsl:for-each>
  </xsl:template>

  <xsl:template name="testAttributes">
    <xsl:attribute name="name">
      <xsl:value-of select="@name"/>
    </xsl:attribute>
    <xsl:attribute name="success">
      <xsl:choose>
        <xsl:when test="@result = 'passed'">True</xsl:when>
        <xsl:when test="@result != 'passed'">False</xsl:when>
      </xsl:choose>
    </xsl:attribute>
    <xsl:attribute name="executed">True</xsl:attribute>
    <xsl:attribute name="time">0</xsl:attribute>
    <xsl:attribute name="asserts">
      <xsl:value-of select="@assertions_failed + @assertions_passed"/>
    </xsl:attribute>
  </xsl:template>

</xsl:stylesheet>